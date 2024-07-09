#ifndef _LED_STATUS_TASK_
#define _LED_STATUS_TASK_

#include <FreeRTOS.h>
#include <queue.h>

#include <sys/types.h>

typedef enum {
    CMD_LSTAT_READY,
    CMD_LSTAT_ERROR,
    CMD_LSTAT_BUSY
} led_status_cmd_type;

typedef struct {
    led_status_cmd_type type;
} led_status_task_cmd;

typedef struct {
    QueueHandle_t cmd_queue;
    uint led_gpio;
} led_status_task_params;

void led_status_task(void *params);

#endif /* _LED_STATUS_TASK_ */