#include "command_hub_task.h"

#include <common_macros.h>

#include <tasks/ic_interfacer_task.h>
#include <tasks/ic_handlers/ic_handlers.h>

typedef enum {
    WAITING_FOR_IC,
    READY,
    ERROR
} command_hub_status;

static void handle_inbound_commands(const command_hub_cmd *cmd, const QueueHandle_t resp_queue);

static void handle_inbound_commands(const command_hub_cmd *cmd, const QueueHandle_t resp_queue) {
    // TODO: Handle commands and respond
}

void command_hub_task(void *params) {
    command_hub_status status = WAITING_FOR_IC;
    TaskHandle_t interfacer_t_handle, cli_interface_t_handle, oled_interface_t_handle;
    
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

    // Start the interfacer task
    xTaskCreate(ic_interfacer_task, "IcInterfacerTask", (configSTACK_DEPTH_TYPE)384, (void*)&intrfc_prms, BASELINE_TASK_PRIORITY, &interfacer_t_handle);

    // Create and start the tasks to handle CLI and OLED interface
    xTaskCreate(ic_interfacer_task, "CLIInterfaceTask", configMINIMAL_STACK_SIZE, (void*)&cli_queues, BASELINE_TASK_PRIORITY, &cli_interface_t_handle);
    xTaskCreate(ic_interfacer_task, "OLEDInterfaceTask", configMINIMAL_STACK_SIZE, (void*)&oled_queues, BASELINE_TASK_PRIORITY, &oled_interface_t_handle);

    while(true) {
        // Receive commands from the CLI
        while(xQueueReceive(cli_queues.cmd_queue, (void*)&(cmd), 0)) {
            handle_inbound_commands(&cmd, cli_queues.resp_queue);
        }

        // Receive commands from the OLED
        while(xQueueReceive(oled_queues.cmd_queue, (void*)&(cmd), 0)) {
            handle_inbound_commands(&cmd, oled_queues.resp_queue);
        }

        while(xQueueReceive(cmd_update_queue, (void*)&(cmd_update), 0)) {
            // TODO: Handle the update, for example saving to SD card or similar

            // Relay command updates to oled and cli tasks
            xQueueSend(cli_queues.cmd_update_queue, (void*)&cmd_update, portMAX_DELAY);    
            xQueueSend(oled_queues.cmd_update_queue, (void*)&cmd_update, portMAX_DELAY);    
        }

        taskYIELD();
    }
}