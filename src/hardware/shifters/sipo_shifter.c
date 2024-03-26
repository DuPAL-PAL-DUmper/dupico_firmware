#include "sipo_shifter.h"
#include <common_macros.h>
#include <hardware/gpio.h>

#include "FreeRTOS.h"
#include "task.h"

static inline void toggle_SRCLK(const SIPO_Config* cfg);
static inline void toggle_RCLK(const SIPO_Config* cfg);

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

    // Set /OE high, everything else low, including /SRCLR, to reset the registers
    gpio_put_masked(_BV(cfg->oe_pin) |
                    _BV(cfg->ser_pin) |
                    _BV(cfg->srclk_pin) |
                    _BV(cfg->rclk_pin) |
                    _BV(cfg->srclr_pin), _BV(cfg->oe_pin));

    vTaskDelay(500);
    gpio_put(cfg->srclr_pin, true); // /SRCLR to high
    vTaskDelay(10);
    gpio_put(cfg->oe_pin, false); // Enable the outputs
}

void sipo_shifter_set(const SIPO_Config* cfg, uint64_t val) {
    for(uint idx = 0; idx < cfg->len; idx++) {
        if((val >> idx) & 0x01) gpio_put(cfg->ser_pin, true); // High
        else gpio_put(cfg->ser_pin, false); // Low

        toggle_SRCLK(cfg);
    }

    toggle_RCLK(cfg); // Send out the values 
}

static inline void toggle_SRCLK(const SIPO_Config* cfg) {
    // This will advance the shift register
    gpio_put(cfg->srclk_pin, true); // set clock to high
    vTaskDelay(10);
    gpio_put(cfg->srclk_pin, false); // set clock to low
}

static inline void toggle_RCLK(const SIPO_Config* cfg) {
    // This will store the data in the shift register
    gpio_put(cfg->rclk_pin, true); // set clock to high
    vTaskDelay(10);
    gpio_put(cfg->rclk_pin, false); // set clock to low
}