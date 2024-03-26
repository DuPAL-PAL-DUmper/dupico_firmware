#include "shifter_io_task.h"
#include <stdbool.h>
#include <stdio.h>
#include <portmacro.h>

void shifter_io_task(void *params) {
    shifter_io_task_params *prms = (shifter_io_task_params*)params;
    shifter_io_task_cmd cmd;
    bool keep_going = true;
    uint64_t val;

    while(keep_going) {
        if(xQueueReceive(prms->cmd_queue, (void*)&cmd, portMAX_DELAY)) {
            switch(cmd.cmd) {
                case WRITE:
                    sipo_shifter_set(&(prms->sipo_cfg), cmd.param);
                case READ:
                    val = piso_shifter_get(&(prms->piso_cfg));
                    xQueueSend(prms->resp_queue, (void*)&val, portMAX_DELAY);
                    break;
                case DIE:
                default:
                    keep_going = false;
                    break;
            }
        }
    }

    vTaskDelete(NULL);
}