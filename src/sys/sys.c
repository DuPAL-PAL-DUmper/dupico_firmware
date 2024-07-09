#include "sys.h"

#include "pico/stdlib.h"
#include "pico/stdio_usb.h"
#include "pico/stdio_uart.h"

void sys_RELAY_init(void) {
    gpio_init(RELAY_ENABLE_GPIO);
    gpio_set_dir(RELAY_ENABLE_GPIO, GPIO_OUT);
    gpio_put(RELAY_ENABLE_GPIO, true); // Active low. Disable the relay.
}

void sys_init(void) {
    sys_RELAY_init();

    // Initialize chosen serial port
    stdio_init_all();
    stdio_filter_driver(&stdio_usb);
}