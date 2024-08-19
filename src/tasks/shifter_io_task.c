#include "shifter_io_task.h"
#include <stdbool.h>
#include <stdio.h>
#include <portmacro.h>

#include <hardware/pio.h>

#include <utils/custom_debug.h>

#include "piso.pio.h"
#include "sipo.pio.h"


void shifter_io_task(void *params) {
    shifter_io_task_params *prms = (shifter_io_task_params*)params;
    shifter_io_task_cmd cmd;
    bool keep_going = true;
    uint64_t val;

    // Initialize the pins
    piso_shifter_init(&(prms->piso_cfg));
    sipo_shifter_init(&(prms->sipo_cfg));

    // Initialize the PIO
    PIO pio = pio0;
    uint piso_offset = pio_add_program(pio, &piso_program);
    uint piso_sm = pio_claim_unused_sm(pio, true);
    uint sipo_offset = pio_add_program(pio, &sipo_program);
    uint sipo_sm = pio_claim_unused_sm(pio, true);
    pio_piso_program_init(pio, piso_sm, piso_offset, 1.0, &(prms->piso_cfg));
    pio_sipo_program_init(pio, sipo_sm, sipo_offset, 1.0, &(prms->sipo_cfg));

    // Task loop
    while(keep_going) {
        if(xQueueReceive(prms->cmd_queue, (void*)&cmd, portMAX_DELAY)) {
            switch(cmd.cmd) {
                case SHF_WRITE:
                    sipo_shifter_set(&(prms->sipo_cfg), cmd.param, pio, sipo_sm);
                    // We fall into the next case. This is not an error!!!
                case SHF_READ:
                    val = piso_shifter_get(&(prms->piso_cfg), pio, piso_sm);
                    xQueueSend(prms->resp_queue, (void*)&val, portMAX_DELAY);
                    break;
                case SHF_READ_OSC: {
                        uint64_t first_read = piso_shifter_get(&(prms->piso_cfg), pio, piso_sm);
                        val = 0;
                        for (uint16_t idx = 0; idx < cmd.param & 0xFFFF; idx++) { // Cap the iterations at 65k
                            val |= first_read ^ piso_shifter_get(&(prms->piso_cfg), pio, piso_sm);
                        }
                        xQueueSend(prms->resp_queue, (void*)&val, portMAX_DELAY);
                    }
                    break;
                case SHF_DIE:
                default:
                    keep_going = false;
                    break;
            }
        }

        taskYIELD();
    }

    vTaskDelete(NULL);
}