#include "cmd_executor_task.h"

void dummy_command(QueueHandle_t resp_queue, uint id, void *params);

typedef void(*cmd_func)(QueueHandle_t, uint, void*);

static cmd_func command_map[] = {
    dummy_command, // DEFINE_IC
    dummy_command, // SET_ADDRESS
    dummy_command, // GET_ADDRESS 
    dummy_command, // SET_DATA
    dummy_command, // GET_DATA
    dummy_command, // SET_SPECIAL
    dummy_command, // GET_SPECIAL
};

void dummy_command(QueueHandle_t resp_queue, uint id, void *params) {
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
                    command_map[cmd.cmd](prms->resp_queue, cmd.param, NULL);
                    break;
            }
        }
    }

    vTaskDelete(NULL);
}