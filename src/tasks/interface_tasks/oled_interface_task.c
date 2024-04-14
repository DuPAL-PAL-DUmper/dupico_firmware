#include "oled_interface_task.h"

#include <task.h>

void oled_interface_task(void *params) {
    while(true) {
        taskYIELD();
    }
}