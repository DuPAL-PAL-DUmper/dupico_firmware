#include "shifter_io_task.h"
#include <stdbool.h>
#include <stdio.h>
#include <portmacro.h>

#include <utils/custom_debug.h>

void shifter_io_task(void *params) {
    shifter_io_task_params *prms = (shifter_io_task_params*)params;
    shifter_io_task_cmd cmd;
    bool keep_going = true;
    uint64_t val;

    // Initialize the pins
    piso_shifter_init(&(prms->piso_cfg));
    sipo_shifter_init(&(prms->sipo_cfg));

    // Task loop
    while(keep_going) {
        if(xQueueReceive(prms->cmd_queue, (void*)&cmd, portMAX_DELAY)) {
            switch(cmd.cmd) {
                case SHF_WRITE:
                    sipo_shifter_set(&(prms->sipo_cfg), cmd.param);
                    // We fall into the next case. This is not an error!!!
                case SHF_READ:
                    val = piso_shifter_get(&(prms->piso_cfg));
                    xQueueSend(prms->resp_queue, (void*)&val, portMAX_DELAY);
                    break;
                case SHF_DIE:
                default:
                    keep_going = false;
                    break;
            }
        }
    }

    vTaskDelete(NULL);
}