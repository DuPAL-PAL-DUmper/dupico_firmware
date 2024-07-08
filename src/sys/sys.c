#include "sys.h"

#include "pico/stdlib.h"

void sys_SIPO_init(void) {
}

void sys_PISO_init(void) {
}

void sys_PILED_init(void) {
    // Initialize LED pin
    gpio_init(LED_PIN);
    gpio_set_dir(LED_PIN, GPIO_OUT);
    gpio_put(LED_PIN, true);
}

void sys_RELAY_init(void) {
    gpio_init(RELAY_ENABLE_PIN);
    gpio_set_dir(RELAY_ENABLE_PIN, GPIO_OUT);
    gpio_put(RELAY_ENABLE_PIN, true); // Active low. Disable the relay.
}

void sys_init(void) {
    sys_RELAY_init();
    sys_PILED_init();
    sys_SIPO_init();
    sys_PISO_init();

    // Initialize chosen serial port
    stdio_init_all();
    
}