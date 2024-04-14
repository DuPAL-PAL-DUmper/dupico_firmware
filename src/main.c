#include <stdio.h>
#include <stdint.h>

#include "pico/types.h"
#include "pico/stdlib.h"

#include <FreeRTOS.h>
#include <task.h>

#include <sys/sys.h>

#include <tasks/command_hub_task.h>
#include <utils/custom_debug.h>

#define MAIN_TASK_PRIORITY				( tskIDLE_PRIORITY + 1UL )

static const uint8_t __in_flash() test_ic_definition[86] = {
	0x32, 0x37, 0x43, 0x36, 0x34, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0xFF, 0xFF, 0x0D, 0x0A, 0x09, 0x08, 0x07, 0x06, 0x05, 0x04,
	0x03, 0x19, 0x18, 0x15, 0x17, 0x02, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x08, 0x0B, 0x0C, 0x0D, 0x0F, 0x10, 0x11, 0x12,
	0x13, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x1C, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x04, 0x01, 0x14, 0x16, 0x1B, 0x00,
	0x00, 0x00, 0x00, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF
};

const uint8_t led_pin = LED_PIN;

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

            D_PRINTF("\t[%3u] \"%15s\" - bPri:%2u cPri:%2u  hw:%4lu\n",
                pxTaskStatusArray[x].xTaskNumber,
                pxTaskStatusArray[x].pcTaskName,
                pxTaskStatusArray[x].uxBasePriority,
                pxTaskStatusArray[x].uxCurrentPriority,
                hwMark);
        }

        vPortFree(pxTaskStatusArray);
    } else {
        D_PRINTF("!!!Failed allocation for task structure!!!\n");
    }

    D_PRINTF("\tHEAP avl: %u, blks: %lu, min: %lu\n",
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

    xTaskCreate(command_hub_task, "CommandHubTask", (configSTACK_DEPTH_TYPE)384, (void*) NULL, MAIN_TASK_PRIORITY, &command_hub_t_handle);

    while(true) {
        // Now loop indefinitely printing debug data
        D_PRINTF("Main task loop\n");
        statusDebug();
        vTaskDelay(10000);
    }
}

void vLaunch(void) {
    TaskHandle_t main_t_handle;
    xTaskCreate(main_task, "MainThread", configMINIMAL_STACK_SIZE, NULL, MAIN_TASK_PRIORITY, &main_t_handle);

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
