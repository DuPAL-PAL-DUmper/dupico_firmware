#include "command_hub_task.h"

#include <task.h>
#include <stdint.h>

#include <common_macros.h>

#include <utils/custom_debug.h>

#include <tasks/interface_tasks/cli_interface_task.h>
#include <tasks/shifter_io_task.h>

typedef enum {
    READY,
    BUSY,
    ERROR
} command_hub_status;

static void handle_inbound_commands(const command_hub_cmd *cmd, const QueueHandle_t resp_queue, command_hub_status *hub_status);
static void handle_inbound_commands_simple_response(uint id, const QueueHandle_t resp_queue, command_hub_cmd_response_type resp, uint32_t data);

static void handle_inbound_commands(const command_hub_cmd *cmd, const QueueHandle_t resp_queue, command_hub_status *hub_status) {
    switch(cmd->type) {
        case CMDH_RESET:
            // TODO: Reset the status of the SIPO/PISO/Socket VCC
            *hub_status = READY;
            // TODO: Reset the statuses of the tasks, both command execution and interface
            handle_inbound_commands_simple_response(cmd->id, resp_queue, CMDH_RESP_OK, 0);
            break;
        default:
            *hub_status = ERROR;
            handle_inbound_commands_simple_response(cmd->id, resp_queue, CMDH_RESP_ERROR, 0);
            break;
    }
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

void command_hub_task(void *params) {

    command_hub_status status = BUSY;
    TaskHandle_t cli_interface_t_handle;
    
    command_hub_cmd cmd;

    // Queues to send the updates to the CLI task
    // Queues to handle reception of commands and responses from CLI task
    command_hub_queues cli_queues = {
        .cmd_queue = xQueueCreate(1, sizeof(command_hub_cmd)),
        .resp_queue = xQueueCreate(1, sizeof(command_hub_cmd_resp))
    };

    shifter_io_task_params shifter_params = {
        .piso_cfg = {

        },
        .sipo_cfg = {

        },
        .cmd_queue = xQueueCreate(1, sizeof(shifter_io_task_cmd)),
        .resp_queue = xQueueCreate(1, sizeof(uint64_t))
    };

    // Create and start the tasks to handle CLI interface
    xTaskCreate(cli_interface_task, "CLIInterfaceTask", configMINIMAL_STACK_SIZE, (void*)&cli_queues, BASELINE_TASK_PRIORITY, &cli_interface_t_handle);

    status = READY;

    while(true) {
        // Receive commands from the CLI
        while(xQueueReceive(cli_queues.cmd_queue, (void*)&(cmd), 0)) {
            handle_inbound_commands(&cmd, cli_queues.resp_queue, &status);
        }

        taskYIELD();
    }
}