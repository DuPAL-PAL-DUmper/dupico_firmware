#include "sys.h"

#include "pico/stdlib.h"
#include "hardware/watchdog.h"
#include "pico/stdio_usb.h"
#include "pico/stdio_uart.h"

#include "custom_debug.h"

#define WATCHDOG_TIMEOUT 2000 // 2 seconds

static void sys_RELAY_init(void);
static void sys_WD_init(void);

static void sys_RELAY_init(void) {
    gpio_init(RELAY_ENABLE_GPIO);
    gpio_set_dir(RELAY_ENABLE_GPIO, GPIO_OUT);
    gpio_put(RELAY_ENABLE_GPIO, true); // Active low. Disable the relay.
}

static void sys_WD_init(void) {
    watchdog_enable(WATCHDOG_TIMEOUT, true);
}

void sys_init(void) {
    sys_WD_init();

    sys_RELAY_init();

    // Initialize chosen serial port
    stdio_init_all();
    stdio_filter_driver(&stdio_usb); // Filter inputs for USB only

    init_print();

    if(watchdog_caused_reboot()) {
        D_PRINTF("Previous reboot was caused by watchdog!!!\r\n");
    }
}