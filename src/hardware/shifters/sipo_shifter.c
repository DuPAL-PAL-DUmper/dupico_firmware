#include "sipo_shifter.h"
#include <common_macros.h>
#include <hardware/gpio.h>

#include <utils/custom_debug.h>

#include "FreeRTOS.h"
#include "task.h"

#define _NOP() asm volatile ( "nop" );

static inline void toggle_RCLK(const SIPO_Config* cfg);

void sipo_shifter_init(const SIPO_Config* cfg) {
    // Init the pins
    gpio_init_mask( _BV(cfg->oe_pin)  |
                    _BV(cfg->rclk_pin) |
                    _BV(cfg->srclr_pin));

    // Set every pin to output
    gpio_set_dir_masked(_BV(cfg->oe_pin)  |
                        _BV(cfg->rclk_pin) |
                        _BV(cfg->srclr_pin),
                        0xFFFFFFFF);

    // Set /OE high, everything else low, including /SRCLR, to reset the registers
    gpio_put_masked(_BV(cfg->oe_pin) |
                    _BV(cfg->rclk_pin) |
                    _BV(cfg->srclr_pin), _BV(cfg->oe_pin));

    vTaskDelay(500);
    gpio_put(cfg->srclr_pin, true); // /SRCLR to high
    vTaskDelay(500);
    gpio_put(cfg->oe_pin, false); // Enable the outputs
}

#define BLK_SIZE 32
void sipo_shifter_set(const SIPO_Config* cfg, uint64_t val, PIO pio, uint sm) {
    // Put the data to shift in the registers in the FIFO queue
    pio_sm_put_blocking(pio, sm, (uint32_t)(val >> BLK_SIZE));
    pio_sm_put_blocking(pio, sm, (uint32_t)(val & 0xFFFFFFFF));

    // Make sure the PIO has finished shifting
    pio_sm_get_blocking(pio, sm);
    pio_sm_get_blocking(pio, sm);

    toggle_RCLK(cfg); // Send out the values 
}

static inline void toggle_RCLK(const SIPO_Config* cfg) {
    // This will store the data in the shift register
    _NOP();
    gpio_put(cfg->rclk_pin, true); // set clock to high
    _NOP();
    gpio_put(cfg->rclk_pin, false); // set clock to low
}
