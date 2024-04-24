#include "command_hub_task.h"

#include <task.h>
#include <stdint.h>

#include "data_structs.h"

// Headers for SD card access
#include "ff_headers.h"
#include "ff_sddisk.h"
#include "ff_stdio.h"
#include "ff_utils.h"

#include <common_macros.h>

#include <utils/custom_debug.h>

#include <tasks/ic_interfacer_task.h>
#include <tasks/interface_tasks/cli_interface_task.h>
#include <tasks/interface_tasks/oled_interface_task.h>
#include <tasks/ic_handlers/ic_handlers.h>

typedef enum {
    WAITING_FOR_IC,
    READY,
    BUSY,
    ERROR
} command_hub_status;

#define SD_NAME "sd0"
#define DEF_EXT ".def"

static const char __in_flash() SD_FS[] = "/" SD_NAME;
static const char __in_flash() SD_DEFS_PATH[] = "/" SD_NAME "/definitions";

static FF_Disk_t *sd_disk = NULL;
static IC_Ctrl_Struct cur_ic_definition;
static handler_funcs ic_handlers;
FF_FindData_t *pxFindStruct = NULL;

static bool initialize_SD_iface(void);
static void handle_inbound_commands(const command_hub_cmd *cmd, const QueueHandle_t resp_queue, command_hub_status *hub_status);
static void handle_inbound_commands_simple_response(uint id, const QueueHandle_t resp_queue, command_hub_cmd_response_type resp, uint32_t data);
static uint count_ic_definitions(void);
static bool ic_definition_search_restart(void);
static bool ic_definition_search_next(void);

static void handle_inbound_commands(const command_hub_cmd *cmd, const QueueHandle_t resp_queue, command_hub_status *hub_status) {
    switch(cmd->type) {
        case CMDH_RESET:
            // Reinit the SD interface
            initialize_SD_iface();
            *hub_status = WAITING_FOR_IC;
            // TODO: Reset the statuses of the tasks, both command execution and interface
            handle_inbound_commands_simple_response(cmd->id, resp_queue, CMDH_RESP_OK, 0);
            break;
        case CMDH_SUPPORTED_IC_COUNT:
            handle_inbound_commands_simple_response(cmd->id, resp_queue, CMDH_RESP_OK, count_ic_definitions());
            break;
        case CMDH_SUPPORTED_IC_BEGIN_LIST:
            // TODO: Point to the first definition of the supported ICs, return it
        case CMDH_SUPPORTED_IC_LIST_NEXT:
            // TODO: Return the next definition on the list of supported ICs. If the list is at the end, return the same
        case CMDH_SUPPORTED_IC_LIST_SELECT:
            // TODO: Select the current IC definition, retrieve the handlers for it
        case CMDH_SELECTED_IC_GET_CMD_LIST:
            if(*hub_status != READY) {
                handle_inbound_commands_simple_response(cmd->id, resp_queue, CMDH_RESP_ERROR, 0);
            }  else {
                uint cmd_list_size;
                const cmd_list_entry* cmds = ic_handlers.get_commands(&cmd_list_size);
                // Return the command list and its size
                xQueueSend(resp_queue, (void*)& ((command_hub_cmd_resp){
                    .id = cmd->id,
                    .type = CMDH_RESP_OK,
                    .data = (command_hub_cmd_resp_data) {
                        .cmdlist = (command_hub_cmd_resp_cmdlist) {
                            .cmds = cmds,
                            .size = cmd_list_size
                        }
                    }
                }), portMAX_DELAY);                
            }
            break;
        case CMDH_SELECTED_IC_EXEC_CMD:
            // TODO: Execute the included command for said IC in a specialized command task
        default:
            *hub_status = ERROR;
            handle_inbound_commands_simple_response(cmd->id, resp_queue, CMDH_RESP_ERROR, 0);
            break;
    }
    // TODO: Commands must be offloaded to another task, which will send updates through the cmd_status_update queue
}

static void handle_inbound_commands_simple_response(uint id, const QueueHandle_t resp_queue, command_hub_cmd_response_type resp, uint32_t data) {
    xQueueSend(resp_queue, (void*)& ((command_hub_cmd_resp){
        .id = id,
        .type = resp,
        .data = (command_hub_cmd_resp_data) {
            .data = data
        }
    }), portMAX_DELAY);
}

static uint count_ic_definitions(void) {
    uint count = 0;

    if(ic_definition_search_restart()) {
        count++;
        while(ic_definition_search_next()) count++;
    }

    return count;
}

static bool ic_definition_search_restart(void) {
    memset(pxFindStruct, 0x00, sizeof(FF_FindData_t));
    if(ff_findfirst(SD_DEFS_PATH, pxFindStruct) == 0) {
        do {
            if(!(pxFindStruct->ucAttributes & FF_FAT_ATTR_DIR)) {
                size_t fname_len = strnlen(pxFindStruct->pcFileName, 30);
                if(pxFindStruct->ulFileSize == sizeof(IC_Ctrl_Struct) && 
                    strncmp(DEF_EXT, pxFindStruct->pcFileName + (fname_len - (sizeof(DEF_EXT) - 1)), sizeof(DEF_EXT) - 1) == 0) { 
                    D_PRINTF("%s [size=%d]\n", pxFindStruct->pcFileName, pxFindStruct->ulFileSize);
                    return true;
                } else { // Skipped!
                    D_PRINTF("%s [size=%d] - SKIPPED!\n", pxFindStruct->pcFileName, pxFindStruct->ulFileSize);
                }
            }
        } while(ff_findnext(pxFindStruct) == 0);
    }

    return false;
}

static bool ic_definition_search_next(void) {
    while(ff_findnext(pxFindStruct) == 0) {
        if(!(pxFindStruct->ucAttributes & FF_FAT_ATTR_DIR)) {
            size_t fname_len = strnlen(pxFindStruct->pcFileName, 30);
            if(pxFindStruct->ulFileSize == sizeof(IC_Ctrl_Struct) && 
                strncmp(DEF_EXT, pxFindStruct->pcFileName + (fname_len - (sizeof(DEF_EXT) - 1)), sizeof(DEF_EXT) - 1) == 0) { 
                D_PRINTF("%s [size=%d]\n", pxFindStruct->pcFileName, pxFindStruct->ulFileSize);
                return true;
            } else { // Skipped!
                D_PRINTF("%s [size=%d] - SKIPPED!\n", pxFindStruct->pcFileName, pxFindStruct->ulFileSize);
            }
        }
    };

    return false;
}

static bool initialize_SD_iface(void) {
    if(sd_disk) {
        FF_FS_Remove(SD_FS);
        FF_SDDiskUnmount(sd_disk);
        FF_SDDiskDelete(sd_disk);
    }

    sd_disk = FF_SDDiskInit(SD_NAME);

    if(!sd_disk) {
        D_PRINTF("Failed to build structure for SD card.\n");
        return false; // Failed initialization
    }

    FF_Error_t xError = FF_SDDiskMount(sd_disk);
    if (FF_isERR(xError) != pdFALSE) {
        D_PRINTF("FF_SDDiskMount: %s\n",
                  (const char *)FF_GetErrMessage(xError));
        return false;
    }
    FF_FS_Add(SD_FS, sd_disk);

    D_PRINTF("Initialized the SD card.\n");

    ff_mkdir(SD_DEFS_PATH);

    if(!pxFindStruct) {
        pxFindStruct = (FF_FindData_t *)pvPortMalloc(sizeof(FF_FindData_t));
    }

    memset(pxFindStruct, 0x00, sizeof(FF_FindData_t));

    return true;
}

void command_hub_task(void *params) {
    sd_disk = NULL;

    command_hub_status status = WAITING_FOR_IC;
    TaskHandle_t /*interfacer_t_handle,*/ cli_interface_t_handle, oled_interface_t_handle;
    
    // Parameters for the IC interfacer task
    ic_interfacer_task_params intrfc_prms = {
        .cmd_queue = xQueueCreate(1, sizeof(ic_interfacer_command)),
        .resp_queue = xQueueCreate(1, sizeof(ic_interfacer_command_response))
    };

    // Handle updates from sent commands
    QueueHandle_t cmd_update_queue = xQueueCreate(2, sizeof(cmd_status_update));
    cmd_status_update cmd_update;
    command_hub_cmd cmd;

    // Queues to send the updates to the CLI and OLED tasks
    // Queues to handle reception of commands and responses from CLI and OLED tasks
    command_hub_queues cli_queues = {
        .cmd_queue = xQueueCreate(1, sizeof(command_hub_cmd)),
        .resp_queue = xQueueCreate(1, sizeof(command_hub_cmd_resp)),
        cmd_update_queue = xQueueCreate(2, sizeof(cmd_status_update))
    };

    command_hub_queues oled_queues = {
        .cmd_queue = xQueueCreate(1, sizeof(command_hub_cmd)),
        .resp_queue = xQueueCreate(1, sizeof(command_hub_cmd_resp)),
        cmd_update_queue = xQueueCreate(2, sizeof(cmd_status_update))
    };

    // Initialize SD Card
    initialize_SD_iface();

    // Start the interfacer task
    // TODO: Maybe this should be handled by a task specialized in command execution
    //xTaskCreate(ic_interfacer_task, "IcInterfacerTask", (configSTACK_DEPTH_TYPE)384, (void*)&intrfc_prms, BASELINE_TASK_PRIORITY, &interfacer_t_handle);

    // Create and start the tasks to handle CLI and OLED interface
    xTaskCreate(oled_interface_task, "CLIInterfaceTask", configMINIMAL_STACK_SIZE, (void*)&cli_queues, BASELINE_TASK_PRIORITY, &cli_interface_t_handle);
    xTaskCreate(cli_interface_task, "OLEDInterfaceTask", configMINIMAL_STACK_SIZE, (void*)&oled_queues, BASELINE_TASK_PRIORITY, &oled_interface_t_handle);

    while(true) {
        // Receive commands from the CLI
        while(xQueueReceive(cli_queues.cmd_queue, (void*)&(cmd), 0)) {
            handle_inbound_commands(&cmd, cli_queues.resp_queue, &status);
        }

        // Receive commands from the OLED
        while(xQueueReceive(oled_queues.cmd_queue, (void*)&(cmd), 0)) {
            handle_inbound_commands(&cmd, oled_queues.resp_queue, &status);
        }

        while(xQueueReceive(cmd_update_queue, (void*)&(cmd_update), 0)) {
            // TODO: Handle the updates coming in from the command (who is sending them???)

            // Relay command updates to oled and cli tasks
            xQueueSend(cli_queues.cmd_update_queue, (void*)&cmd_update, portMAX_DELAY);    
            xQueueSend(oled_queues.cmd_update_queue, (void*)&cmd_update, portMAX_DELAY);    
        }

        taskYIELD();
    }
}