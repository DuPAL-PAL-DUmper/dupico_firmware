#include <stdio.h>
#include <stdint.h>

#include "pico/types.h"
#include "pico/stdlib.h"

#include "FreeRTOS.h"
#include "task.h"

#include "ff_headers.h"
#include "ff_sddisk.h"
#include "ff_stdio.h"
#include "ff_utils.h"

#include <sys/sys.h>

#ifndef RUN_FREERTOS_ON_CORE
#define RUN_FREERTOS_ON_CORE 0
#endif

#define TEST_TASK_PRIORITY				( tskIDLE_PRIORITY + 2UL )
#define BLINK_TASK_PRIORITY				( tskIDLE_PRIORITY + 1UL )


const uint8_t led_pin = LED_PIN;

static inline void stop() {
    fflush(stdout);
    __breakpoint();
}

void main_task(__unused void *params) {
    while(true) {
        // not much to do for now
        vTaskDelay(10000);
    }
}

void vLaunch( void) {
    TaskHandle_t task;
    xTaskCreate(main_task, "MainThread", configMINIMAL_STACK_SIZE, NULL, TEST_TASK_PRIORITY, &task);

#if NO_SYS && configUSE_CORE_AFFINITY && configNUM_CORESconfigNUMBER_OF_CORES > 1
    // we must bind the main task to one core (well at least while the init is called)
    // (note we only do this in NO_SYS mode, because cyw43_arch_freertos
    // takes care of it otherwise)
    vTaskCoreAffinitySet(task, 1);
#endif

    /* Start the tasks and timer running. */
    vTaskStartScheduler();
}

int main() {
    // Hardware initialization
    sys_init();

    /* Configure the hardware ready to run the demo. */
#if ( FREE_RTOS_KERNEL_SMP == 1 ) && ( configNUMBER_OF_CORES == 2 )
    vLaunch();
#else
#error "SMP not enabled!"
#endif
    return 0;
}

void vApplicationPassiveIdleHook(void) {
    return;
}
