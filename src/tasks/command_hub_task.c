#include "command_hub_task.h"

#include <common_macros.h>

#include <tasks/ic_interfacer_task.h>
#include <tasks/ic_handlers/ic_handlers.h>

typedef enum {
    WAITING_FOR_IC,
    READY,
    ERROR
} command_hub_status;

void command_hub_task(void *params) {
    command_hub_status status = WAITING_FOR_IC;
    TaskHandle_t interfacer_t_handle;
    
    // Parameters for the IC interfacer task
    ic_interfacer_task_params intrfc_prms = {
        .cmd_queue = xQueueCreate(1, sizeof(ic_interfacer_command)),
        .resp_queue = xQueueCreate(1, sizeof(ic_interfacer_command_response))
    };

    // Handle updates from sent commands
    QueueHandle_t cmd_update_queue = xQueueCreate(2, sizeof(cmd_status_update));
    cmd_status_update cmd_update;

    // Queues to send the updates to the CLI and OLED tasks
    // Queues to handle reception of commands and responses from CLI and OLED tasks
    command_hub_queues cli_queues = {
        .cmd_queue = xQueueCreate(1, sizeof(command_hub_command)),
        .resp_queue = xQueueCreate(1, sizeof(command_hub_command_response)),
        cmd_update_queue = xQueueCreate(2, sizeof(cmd_status_update))
    };

    command_hub_queues oled_queues = {
        .cmd_queue = xQueueCreate(1, sizeof(command_hub_command)),
        .resp_queue = xQueueCreate(1, sizeof(command_hub_command_response)),
        cmd_update_queue = xQueueCreate(2, sizeof(cmd_status_update))
    };

    // Start the interfacer task
    xTaskCreate(ic_interfacer_task, "IcInterfacerTask", (configSTACK_DEPTH_TYPE)384, (void*)&intrfc_prms, BASELINE_TASK_PRIORITY, &interfacer_t_handle);

    // TODO: Create and start the tasks to handle CLI and OLED interface
    // TODO: Define the interface to send commands and receive responses from this task
    // TODO: Define the queues to handle the above

    while(true) {

        while(xQueueReceive(cmd_update_queue, (void*)&(cmd_update), 0)) {
            // TODO: Handle and redirect updates from commands
        }

        vTaskDelay(10000);
    }
}