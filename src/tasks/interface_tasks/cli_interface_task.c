#include "cli_interface_task.h"

#include "pico/stdio_usb.h"

#include <task.h>

#include "utils/custom_debug.h"

void cli_interface_task(void *params) {
    while(true) {
        retarg_printf((void *)&stdio_usb, "Cieo!\r\n");
        vTaskDelay(10000);
    } 
}