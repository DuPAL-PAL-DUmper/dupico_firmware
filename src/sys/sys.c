#include "sys.h"

#include "pico/stdlib.h"
#include "hardware/i2c.h"

#include <hardware/ssd1306_i2c/ssd1306_i2c.h>

void sys_i2c_init(void) {
    i2c_init(i2c_default, SSD1306_I2C_CLK * 1000);
    gpio_set_function(PICO_DEFAULT_I2C_SDA_PIN, GPIO_FUNC_I2C);
    gpio_set_function(PICO_DEFAULT_I2C_SCL_PIN, GPIO_FUNC_I2C);
    gpio_pull_up(PICO_DEFAULT_I2C_SDA_PIN);
    gpio_pull_up(PICO_DEFAULT_I2C_SCL_PIN);
}

void sys_init(void) {
    // Initialize LED pin
    gpio_init(LED_PIN);
    gpio_set_dir(LED_PIN, GPIO_OUT);

    // Initialize chosen serial port
    stdio_init_all();

    //sys_i2c_init();
    //SSD1306_init();
}