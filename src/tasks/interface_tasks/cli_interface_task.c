#include "cli_interface_task.h"

#include <task.h>

void cli_interface_task(void *params) {
    while(true) {
        taskYIELD();
    } 
}