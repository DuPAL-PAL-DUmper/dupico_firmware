#include <stdio.h>
#include <stdint.h>

#include "pico/types.h"
#include "pico/stdlib.h"

#include <FreeRTOS.h>
#include <task.h>
#include <queue.h>

#include "ff_headers.h"
#include "ff_sddisk.h"
#include "ff_stdio.h"
#include "ff_utils.h"

#include <sys/sys.h>

#include <tasks/ic_interfacer_task.h>
#include <utils/custom_debug.h>

#define MAIN_TASK_PRIORITY				( tskIDLE_PRIORITY + 1UL )

static const uint8_t test_ic_definition[83] = {
	0x32, 0x37, 0x43, 0x36, 0x34, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x0D, 0x0A, 0x09, 0x08, 0x07, 0x06, 0x05, 0x04,
	0x03, 0x19, 0x18, 0x15, 0x17, 0x02, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x08, 0x0B, 0x0C, 0x0D, 0x0F, 0x10, 0x11, 0x12,
	0x13, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x1C, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x04, 0x01, 0x14, 0x16, 0x1B, 0x00,
	0x00, 0x00, 0x00, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF
};

const uint8_t led_pin = LED_PIN;

static inline void stop() {
    fflush(stdout);
    __breakpoint();
}


void consumer_task(__unused void *params) {
    ic_interfacer_task_params *prms = (ic_interfacer_task_params*)params;

    ic_interfacer_command define_ic_cmd = {
        .cmd = INTF_DEFINE_IC,
        .id = 1,
        .param = test_ic_definition
    };
    ic_interfacer_command_response resp;

    xQueueSend(prms->cmd_queue, (void*)&define_ic_cmd, portMAX_DELAY);
    if(xQueueReceive(prms->resp_queue, (void*)&(resp), portMAX_DELAY)) { 
    }

    while(true) {
        // not much to do for now
        D_PRINTF("Consumer task loop\n");
        vTaskDelay(10000);
    }
}

void main_task(__unused void *params) {
    TaskHandle_t consumer_t_handle, interfacer_t_handle;
    ic_interfacer_task_params intrfc_prms = {
        .cmd_queue = xQueueCreate(1, sizeof(ic_interfacer_command)),
        .resp_queue = xQueueCreate(1, sizeof(ic_interfacer_command_response))
    };

    xTaskCreate(consumer_task, "ConsumerTask", configMINIMAL_STACK_SIZE, (void*)&intrfc_prms, MAIN_TASK_PRIORITY, &consumer_t_handle);
    xTaskCreate(ic_interfacer_task, "IcInterfacerTask", configMINIMAL_STACK_SIZE, (void*)&intrfc_prms, MAIN_TASK_PRIORITY, &interfacer_t_handle);

    while(true) {
        // not much to do for now
        D_PRINTF("Main task loop\n");
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
