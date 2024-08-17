#include "sipo_shifter.h"
#include <common_macros.h>
#include <hardware/gpio.h>

#include <utils/custom_debug.h>

#include "FreeRTOS.h"
#include "task.h"

void sipo_shifter_init(const SIPO_Config* cfg) {
    // Empty husk left after moving everything to PIO
}

#define BLK_SIZE 32
void sipo_shifter_set(const SIPO_Config* cfg, uint64_t val, PIO pio, uint sm) {
    // Put the data to shift in the registers in the FIFO queue
    pio_sm_put_blocking(pio, sm, (uint32_t)(val >> BLK_SIZE));
    pio_sm_put_blocking(pio, sm, (uint32_t)(val & 0xFFFFFFFF));

    // Make sure the PIO has finished shifting
    pio_sm_get_blocking(pio, sm);
}
