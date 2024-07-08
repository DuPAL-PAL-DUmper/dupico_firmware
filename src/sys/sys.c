#include "sys.h"

#include "pico/stdlib.h"

void sys_PILED_init(void) {
    // Initialize LED pin
    gpio_init(LED_GPIO);
    gpio_set_dir(LED_GPIO, GPIO_OUT);
    gpio_put(LED_GPIO, true);
}

void sys_RELAY_init(void) {
    gpio_init(RELAY_ENABLE_GPIO);
    gpio_set_dir(RELAY_ENABLE_GPIO, GPIO_OUT);
    gpio_put(RELAY_ENABLE_GPIO, true); // Active low. Disable the relay.
}

void sys_init(void) {
    sys_RELAY_init();
    sys_PILED_init();

    // Initialize chosen serial port
    stdio_init_all();
    
}