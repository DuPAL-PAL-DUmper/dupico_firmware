
#include "piso_shifter.h"
#include <hardware/gpio.h>

#include "FreeRTOS.h"
#include "task.h"

void piso_shifter_init(PISO_Config* cfg) {
    // Init the pins
    gpio_init_mask( cfg->ser_pin |
                    cfg->ce_pin  |
                    cfg->pe_pin  |
                    cfg->clk_pin |
                    cfg->clr_pin);

    // Set every pin to output, except the ser_pin, which is input
    gpio_set_dir_masked(cfg->ser_pin |
                    cfg->ce_pin  |
                    cfg->pe_pin  |
                    cfg->clk_pin |
                    cfg->clr_pin,
                    cfg->ce_pin  |
                    cfg->pe_pin  |
                    cfg->clk_pin |
                    cfg->clr_pin);

    // Set /CE and /PE to high, all the rest to low
    gpio_put_masked(cfg->ce_pin  |
                    cfg->pe_pin  |
                    cfg->clk_pin |
                    cfg->clr_pin,
                    cfg->ce_pin  |
                    cfg->pe_pin);

    vTaskDelay(500);

    // Clear the reset
    gpio_put(cfg->clr_pin, true);
}

uint64_t piso_shifter_get(PISO_Config* cfg) {
    uint64_t data = 0;

    // Enable clock and inputs, then set clock to low
    gpio_put_masked(cfg->ce_pin | cfg->pe_pin | cfg->clk_pin, 0);
    vTaskDelay(10);

    // Set the clock to high
    gpio_put(cfg->clk_pin, true);
    vTaskDelay(10);
    // Disable the inputs
    gpio_put(cfg->pe_pin, true);

    for (uint idx = 0; idx < cfg->len; idx++) {
        vTaskDelay(10);
        gpio_put(cfg->clk_pin, true); // Clock out the data
        vTaskDelay(10);
        
        data |= gpio_get(cfg->ser_pin) ? (1 << idx) : 0;
        
        gpio_put(cfg->clk_pin, false); 
    }

    // Disable the clock and inputs
    gpio_put_masked(cfg->ce_pin | cfg->pe_pin, 0xFFFFFFFF);

    return data;
}