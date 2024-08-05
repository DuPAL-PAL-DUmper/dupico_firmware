#include "sipo_shifter.h"
#include <common_macros.h>
#include <hardware/gpio.h>

#include "FreeRTOS.h"
#include "task.h"

#define _NOP() asm volatile ( "nop" );

static inline void toggle_SRCLK(const SIPO_Config* cfg);
static inline void toggle_RCLK(const SIPO_Config* cfg);
static inline void nop_delay(uint16_t delay);

void sipo_shifter_init(const SIPO_Config* cfg) {
    // Init the pins
    gpio_init_mask( _BV(cfg->ser_pin) |
                    _BV(cfg->oe_pin)  |
                    _BV(cfg->srclk_pin) |
                    _BV(cfg->rclk_pin) |
                    _BV(cfg->srclr_pin));

    // Set every pin to output
    gpio_set_dir_masked(_BV(cfg->ser_pin) |
                        _BV(cfg->oe_pin)  |
                        _BV(cfg->srclk_pin) |
                        _BV(cfg->rclk_pin) |
                        _BV(cfg->srclr_pin),
                        0xFFFFFFFF);

    // There's some load on this pin (multiple shift registers, plus a led)
    gpio_set_slew_rate(cfg->srclk_pin, GPIO_SLEW_RATE_FAST);
    gpio_set_drive_strength(cfg->srclk_pin, GPIO_DRIVE_STRENGTH_8MA);

    // Set /OE high, everything else low, including /SRCLR, to reset the registers
    gpio_put_masked(_BV(cfg->oe_pin) |
                    _BV(cfg->ser_pin) |
                    _BV(cfg->srclk_pin) |
                    _BV(cfg->rclk_pin) |
                    _BV(cfg->srclr_pin), _BV(cfg->oe_pin));

    vTaskDelay(500);
    gpio_put(cfg->srclr_pin, true); // /SRCLR to high
    nop_delay(200);
    gpio_put(cfg->oe_pin, false); // Enable the outputs
}

void sipo_shifter_set(const SIPO_Config* cfg, uint64_t val) {
    for(uint idx = 0; idx < cfg->len; idx++) {
        gpio_put(cfg->ser_pin, ((val >> (cfg->len - (idx + 1))) & 0x01)); 
        toggle_SRCLK(cfg);
    }

    toggle_RCLK(cfg); // Send out the values 
}

static inline void toggle_SRCLK(const SIPO_Config* cfg) {
    // This will advance the shift register
    nop_delay(5);
    gpio_put(cfg->srclk_pin, true); // set clock to high
    nop_delay(5);
    gpio_put(cfg->srclk_pin, false); // set clock to low
    nop_delay(5);
}

static inline void toggle_RCLK(const SIPO_Config* cfg) {
    // This will store the data in the shift register
    nop_delay(5);
    gpio_put(cfg->rclk_pin, true); // set clock to high
    nop_delay(5);
    gpio_put(cfg->rclk_pin, false); // set clock to low
}

static inline void nop_delay(uint16_t delay) {
    while(delay--) _NOP();
}