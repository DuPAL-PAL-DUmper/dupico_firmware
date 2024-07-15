#include <stdio.h>
#include <stdint.h>

#include "pico/types.h"
#include "pico/stdlib.h"

#include <FreeRTOS.h>
#include <task.h>

#include <sys/sys.h>

#include <tasks/command_hub_task.h>

#include <common_macros.h>
#include <utils/custom_debug.h>

const uint8_t led_pin = LED_GPIO;

static void statusDebug() {
    uint16_t tot_tasks = uxTaskGetNumberOfTasks();
    UBaseType_t hwMark;
    HeapStats_t heapStats;
    // Allocate a TaskStatus_t structure for each task.  An array could be allocated statically at compile time.
    TaskStatus_t *pxTaskStatusArray = pvPortMalloc(tot_tasks * sizeof(TaskStatus_t));

    vPortGetHeapStats(&heapStats);

    if(pxTaskStatusArray != NULL) {
        tot_tasks = uxTaskGetSystemState(pxTaskStatusArray, tot_tasks, NULL);

        for(uint16_t x = 0; x < tot_tasks; x++) {
            // What percentage of the total run time has the task used?
            // This will always be rounded down to the nearest integer.
            // ulTotalRunTime has already been divided by 100.
            hwMark = uxTaskGetStackHighWaterMark(pxTaskStatusArray[x].xHandle);

            D_PRINTF("\t[%3u] \"%15s\" - bPri:%2u cPri:%2u  hw:%4lu\r\n",
                pxTaskStatusArray[x].xTaskNumber,
                pxTaskStatusArray[x].pcTaskName,
                pxTaskStatusArray[x].uxBasePriority,
                pxTaskStatusArray[x].uxCurrentPriority,
                hwMark);
        }

        vPortFree(pxTaskStatusArray);
    } else {
        D_PRINTF("!!!Failed allocation for task structure!!!\r\n");
    }

    D_PRINTF("\tHEAP avl: %u, blks: %lu, min: %lu\r\n",
        heapStats.xAvailableHeapSpaceInBytes,
        heapStats.xNumberOfFreeBlocks,
        heapStats.xMinimumEverFreeBytesRemaining);
}

static inline void stop() {
    fflush(stdout);
    __breakpoint();
}


void main_task(__unused void *params) {
    TaskHandle_t command_hub_t_handle;

    D_PRINTF("Main task startup!!!\r\n\n");

    xTaskCreate(command_hub_task, "CommandHubTask", configMINIMAL_STACK_SIZE * 2, (void*) NULL, BASELINE_TASK_PRIORITY, &command_hub_t_handle);

    while(true) {
#if DEBUG == 1
        // Now loop indefinitely printing debug data
        D_PRINTF("Main task loop\r\n");
        statusDebug();
#endif
        vTaskDelay(10000);
    }
}

void vLaunch(void) {
    TaskHandle_t main_t_handle;
    xTaskCreate(main_task, "MainThread", configMINIMAL_STACK_SIZE, NULL, BASELINE_TASK_PRIORITY, &main_t_handle);

#if NO_SYS && configUSE_CORE_AFFINITY && configNUMBER_OF_CORES > 1
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

void vApplicationStackOverflowHook(TaskHandle_t pxTask, char *pcTaskName )
{
    while (true) {
        D_PRINTF("!!! STACK OVERFLOW on %s\n\n", pcTaskName);
    }
}


// also check vAssertCalled
