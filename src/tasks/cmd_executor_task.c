#include "cmd_executor_task.h"

void dummy_command(QueueHandle_t resp_queue, void *params);

typedef void(*cmd_func)(QueueHandle_t, void*);

static cmd_func command_map[] = {
    dummy_command, // READ
    dummy_command, // WRITE
    dummy_command, // GET_POWER
    dummy_command, // SET_POWER
    dummy_command, // DUMP_TO_SD
    dummy_command, // STREAM_FROM_SD
};

void dummy_command(QueueHandle_t resp_queue, void *params) {
    return;
}

void cmd_executor_task(void *params) {
    cmd_executor_task_params *prms = (cmd_executor_task_params*)params;
    cmd_executor_command cmd;
    bool keep_going = true;

    while(keep_going) {
        if(xQueueReceive(prms->cmd_queue, (void*)&cmd, portMAX_DELAY)) {
            switch(cmd.cmd) {
                case DIE:
                    keep_going = false;
                    break;
                default:
                    command_map[cmd.cmd](prms->resp_queue, NULL); // TODO: Add some kind of parameter
                    break;
            }
        }
    }

    vTaskDelete(NULL);
}