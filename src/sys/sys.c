#include "sys.h"

#include "pico/stdlib.h"
#include "hardware/watchdog.h"
#include "pico/stdio_usb.h"
#include "pico/stdio_uart.h"

#define WATCHDOG_TIMEOUT 3000 // 3 seconds

void sys_RELAY_init(void) {
    gpio_init(RELAY_ENABLE_GPIO);
    gpio_set_dir(RELAY_ENABLE_GPIO, GPIO_OUT);
    gpio_put(RELAY_ENABLE_GPIO, true); // Active low. Disable the relay.
}

void sys_WD_init(void) {
    watchdog_enable(WATCHDOG_TIMEOUT, true);
}

void sys_init(void) {
    sys_WD_init();

    sys_RELAY_init();

    // Initialize chosen serial port
    stdio_init_all();
    stdio_filter_driver(&stdio_usb); // Filter inputs for USB only
}