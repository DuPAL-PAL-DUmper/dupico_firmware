
#include "piso_shifter.h"
#include <common_macros.h>
#include <hardware/gpio.h>

#include "FreeRTOS.h"
#include "task.h"

#include "utils/custom_debug.h"

#define _NOP() asm volatile ( "nop" );

void piso_shifter_init(const PISO_Config* cfg) {
    // Init the pins
    gpio_init_mask( _BV(cfg->ser_pin));

    // Set every pin to output, except the ser_pin, which is input
    gpio_set_dir_masked(_BV(cfg->ser_pin), 0);
    
    // Set a pullup on the input
    gpio_pull_up(cfg->ser_pin);
}

#define BLK_SIZE 32
uint64_t piso_shifter_get(const PISO_Config* cfg, PIO pio, uint sm) {    
    // "volatile" here is needed because apparently gcc is doing some optimization
    // fuckery that breaks the return value.
    volatile uint64_t data = 0;

    pio_sm_put_blocking(pio, sm, (2 * BLK_SIZE) - 1); // PIO code is structured as a do {} while for this
        
    data = ((uint64_t)pio_sm_get_blocking(pio, sm));        
    data <<= BLK_SIZE;
    data |= ((uint64_t)pio_sm_get_blocking(pio, sm));        
    
    return (data >> 24) & 0xFFFFFFFFFF;
}
