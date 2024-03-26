#ifndef _CMD_EXECUTOR_TASK_
#define _CMD_EXECUTOR_TASK_

#include <FreeRTOS.h>
#include <queue.h>

typedef struct {
    const QueueHandle_t cmd_queue;
    const QueueHandle_t resp_queue;
} cmd_executor_task_params;

void cmd_executor_task(void *params);

#endif /* _CMD_EXECUTOR_TASK_ */