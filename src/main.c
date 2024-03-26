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

#include <tasks/shifter_io_task.h>

#define MAIN_TASK_PRIORITY				( tskIDLE_PRIORITY + 1UL )


const uint8_t led_pin = LED_PIN;

static inline void stop() {
    fflush(stdout);
    __breakpoint();
}


void consumer_task(__unused void *params) {
    shifter_io_task_params *prms = (shifter_io_task_params*)params;


    while(true) {
        // not much to do for now
        printf("Consumer task loop\n");
        shifter_io_task_cmd cmd = {
            .cmd = WRITE,
            .param = 0
        };
        uint64_t val;

        xQueueSend(prms->cmd_queue, (void*)&cmd, portMAX_DELAY);

        if(xQueueReceive(prms->resp_queue, (void*)&val, portMAX_DELAY)) {
            printf("Consumer got %.16X\n", val);
        }

        vTaskDelay(10000);
    }
}

void main_task(__unused void *params) {
    TaskHandle_t consumer_t_handle, shf_task_handle;
    shifter_io_task_params shf_prms = {
        .piso_cfg = {
            .ce_pin = 28,
            .pe_pin = 27,
            .clk_pin = 26,
            .clr_pin = 22,
            .ser_pin = 21,
            .len = 64
        },
        .sipo_cfg = {
            .oe_pin = 20,
            .ser_pin = 19,
            .srclk_pin = 18,
            .rclk_pin = 17,
            .srclr_pin = 16,
            .len = 64
        },
        .cmd_queue = xQueueCreate(3, sizeof(shifter_io_task_cmd)),
        .resp_queue = xQueueCreate(3, sizeof(uint64_t))
    };

    piso_shifter_init(&shf_prms.piso_cfg);
    sipo_shifter_init(&shf_prms.sipo_cfg);

    xTaskCreate(shifter_io_task, "ShifterThread", configMINIMAL_STACK_SIZE, (void*)&shf_prms, MAIN_TASK_PRIORITY, &shf_task_handle);
    xTaskCreate(consumer_task, "ConsumerThread", configMINIMAL_STACK_SIZE, (void*)&shf_prms, MAIN_TASK_PRIORITY, &consumer_t_handle);

    while(true) {
        // not much to do for now
        printf("Main task loop\n");
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
