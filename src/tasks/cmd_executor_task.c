#include "cmd_executor_task.h"

void dummy_command(QueueHandle_t resp_queue, uint id, void *params);

typedef void(*cmd_func)(QueueHandle_t, uint, void*);

static cmd_func command_map[] = {
    dummy_command, // READ_RAW
    dummy_command, // WRITE_RAW
    dummy_command, // DUMP_TO_SD
    dummy_command, // STREAM_FROM_SD
    dummy_command, // SRAM_CHECK
    dummy_command, // DRAM_CHECK
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
                    command_map[cmd.cmd](prms->resp_queue, 0, NULL); // TODO: Add some kind of parameter
                    break;
            }
        }
    }

    vTaskDelete(NULL);
}