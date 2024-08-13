
#include "piso_shifter.h"
#include <common_macros.h>
#include <hardware/gpio.h>

#include "FreeRTOS.h"
#include "task.h"

#include "utils/custom_debug.h"

#define _NOP() asm volatile ( "nop" );

static inline void nop_delay(uint16_t delay);

void piso_shifter_init(const PISO_Config* cfg) {
    // Init the pins
    gpio_init_mask( _BV(cfg->ser_pin) |
                    _BV(cfg->ce_pin)  |
                    _BV(cfg->pe_pin)  |
                    _BV(cfg->clk_pin) |
                    _BV(cfg->clr_pin));

    // Set every pin to output, except the ser_pin, which is input
    gpio_set_dir_masked(_BV(cfg->ser_pin) |
                    _BV(cfg->ce_pin)  |
                    _BV(cfg->pe_pin)  |
                    _BV(cfg->clk_pin) |
                    _BV(cfg->clr_pin),
                    _BV(cfg->ce_pin)  |
                    _BV(cfg->pe_pin)  |
                    _BV(cfg->clk_pin) |
                    _BV(cfg->clr_pin));

    gpio_set_slew_rate(cfg->clk_pin, GPIO_SLEW_RATE_FAST);
    
    // Set a pullup on the input
    gpio_pull_up(cfg->ser_pin);

    // Set /CE and /PE to high, all the rest to low
    gpio_put_masked(_BV(cfg->ce_pin)  |
                    _BV(cfg->pe_pin)  |
                    _BV(cfg->clk_pin) |
                    _BV(cfg->clr_pin),
                    _BV(cfg->ce_pin)  |
                    _BV(cfg->pe_pin));

    vTaskDelay(500);

    // Clear the reset
    gpio_put(cfg->clr_pin, true);

    nop_delay(5);
}

uint64_t piso_shifter_get(const PISO_Config* cfg) {
    uint64_t data = 0;

    // Enable clock and inputs, then set clock to low
    gpio_put_masked(_BV(cfg->ce_pin) | _BV(cfg->pe_pin) | _BV(cfg->clk_pin), 0);
    nop_delay(1);

    // Set the clock to high
    gpio_put(cfg->clk_pin, true);
    nop_delay(1);
    
    // Disable the inputs
    gpio_put(cfg->pe_pin, true);

    for (uint idx = 0; idx < cfg->len; idx++) {
        gpio_put(cfg->clk_pin, true); // Clock out the data
        nop_delay(5);

        data |= gpio_get(cfg->ser_pin) ? (((uint64_t)1) << (cfg->len - (idx + 1))) : 0;
        
        gpio_put(cfg->clk_pin, false); 
        nop_delay(1);
    }

    // Disable the clock and inputs
    gpio_put_masked(_BV(cfg->ce_pin) | _BV(cfg->pe_pin), 0xFFFFFFFF);

    return data;
}

static inline void nop_delay(uint16_t delay) {
    while(delay--) _NOP();
}