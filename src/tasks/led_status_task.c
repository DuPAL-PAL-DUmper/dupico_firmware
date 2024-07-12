#include "led_status_task.h"

#include "pico/stdlib.h" 

#define CONNECTED_DELAY_OFF 300
#define CONNECTED_DELAY_ON 300

#define WAITING_DELAY_OFF 1000
#define WAITING_DELAY_ON 100

#define ERROR_DELAY_ON 1000
#define ERROR_DELAY_OFF 1000

void led_status_task(void *params) {
    led_status_task_params *prms = (led_status_task_params*)params;
    led_status_task_cmd cmd = {
        .type = CMD_LSTAT_WAITING // We'll start in a "waiting" state
    };
    TickType_t delay = CONNECTED_DELAY_OFF;

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
            case CMD_LSTAT_WAITING:
                delay = led_on ? WAITING_DELAY_OFF : WAITING_DELAY_ON; 
                break;
            case CMD_LSTAT_CONNECTED:
                delay = led_on ? CONNECTED_DELAY_OFF : CONNECTED_DELAY_ON; 
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