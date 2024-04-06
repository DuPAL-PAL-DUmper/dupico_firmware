#include "generic_ic_handler.h"

#include <pico/platform.h>

#include <tasks/ic_interfacer_task.h>
#include "basic_handlers_io.h"

#include <utils/custom_debug.h>

typedef const cmd_list_entry* (*cmd_list_getter)(uint*); // Parameter is pointing to a value that is going to be set to the length of the list

enum Command_List {
    CMD_SET_I,
    CMD_SET_IO,
    CMD_GET_IO,
    CMD_SET_CTRL,
    CMD_SET_PWR,
    CMD_COMMIT
};

static const cmd_list_entry __in_flash() cmd_entries[] = {
    {
        .name = "SET Inputs",
        .id = CMD_SET_I,
        .type = CMD_TYPE_CLI_ONLY
    },
    {
        .name = "SET I/Os",
        .id = CMD_SET_IO,
        .type = CMD_TYPE_CLI_ONLY
    },
    {
        .name = "GET I/Os",
        .id = CMD_GET_IO,
        .type = CMD_TYPE_CLI_ONLY
    },
    {
        .name = "SET Controls",
        .id = CMD_SET_CTRL,
        .type = CMD_TYPE_CLI_ONLY
    },
    {
        .name = "SET Power",
        .id = CMD_SET_PWR,
        .type = CMD_TYPE_CLI_ONLY
    },
    {
        .name = "COMMIT",
        .id = CMD_COMMIT,
        .type = CMD_TYPE_CLI_ONLY
    }
};

static void executor(const cmd_list_entry *cmd, const IC_Ctrl_Struct *ic_ctrl, const ic_interfacer_task_params *interfacer_params, const QueueHandle_t update_queue, const void* param) {
    D_PRINTF("Got command %u with name \"%s\"\n", cmd->id, cmd->name);

    if(update_queue) {
        xQueueSend(update_queue, (void*)&((cmd_status_update){
            .cmd = *cmd,
            .status = CMD_STATUS_STARTING,
            .data = 0
        }), portMAX_DELAY);
    }

    switch(cmd->id) {
        case CMD_SET_I:
            handler_set_inputs(interfacer_params, *(uint32_t*)param);
            break;
        case CMD_SET_IO:
            handler_set_io(interfacer_params, *(uint16_t*)param);
            break;
        case CMD_GET_IO:
            *(uint16_t*)param = handler_get_io(interfacer_params);
            break;
        case CMD_SET_CTRL:
            handler_set_control(interfacer_params, *(uint8_t*)param);
            break;
        case CMD_SET_PWR:
            handler_set_power(interfacer_params, *(uint8_t*)param);
            break;
        case CMD_COMMIT:
            handler_commit(interfacer_params);
            break;
        default:
            if(update_queue) {
                xQueueSend(update_queue, (void*)&((cmd_status_update){
                    .cmd = *cmd,
                    .status = CMD_STATUS_FAILED,
                    .data = 0
                }), portMAX_DELAY);
            }
            return;
    }

    if(update_queue) {
        xQueueSend(update_queue, (void*)&((cmd_status_update){
            .cmd = *cmd,
            .status = CMD_STATUS_DONE,
            .data = 0
        }), portMAX_DELAY);
    }
}

static const cmd_list_entry* get_cmd_list(uint* len) {
    *len = sizeof(cmd_entries)/sizeof(cmd_list_entry);
    return cmd_entries;
}

const handler_funcs build_generic_ic_handler() {
    return (handler_funcs){
        .exec_command = executor,
        .get_commands = get_cmd_list
    };
}