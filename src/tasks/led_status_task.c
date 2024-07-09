#include "led_status_task.h"

#include "pico/stdlib.h"

#define READY_DELAY_OFF 1000
#define READY_DELAY_ON 100

#define ERROR_DELAY_ON 1000
#define ERROR_DELAY_OFF 1000

void led_status_task(void *params) {
    led_status_task_params *prms = (led_status_task_params*)params;
    led_status_task_cmd cmd = {
        .type = CMD_LSTAT_READY
    };
    TickType_t delay = READY_DELAY_OFF;

    // Initialize the led pin
    gpio_init(prms->led_gpio);
    gpio_set_dir(prms->led_gpio, GPIO_OUT);
    gpio_put(prms->led_gpio, false);

    while(true) {
        xQueueReceive(prms->cmd_queue, (void*)&cmd, 0);
        
        bool led_on = gpio_get(prms->led_gpio);
        // Flip the LED state
        gpio_put(prms->led_gpio, !led_on);

        switch(cmd.type) {
            case CMD_LSTAT_READY:
                delay = led_on ? READY_DELAY_OFF : READY_DELAY_ON; 
                break;
            case CMD_LSTAT_ERROR:
            default:
                delay = led_on ? ERROR_DELAY_OFF : ERROR_DELAY_ON; 
                break;
        }

        vTaskDelay(delay);
    }

    vTaskDelete(NULL);    
}