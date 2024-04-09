#ifndef _COMMAND_HUB_TASK_
#define _COMMAND_HUB_TASK_

#include <FreeRTOS.h>
#include <queue.h>

typedef struct {
} command_hub_command;

typedef struct {
} command_hub_command_response;

typedef struct {
    QueueHandle_t cmd_queue;
    QueueHandle_t resp_queue;
    QueueHandle_t cmd_update_queue;
} command_hub_queues;

void command_hub_task(void *params);

#endif /* _COMMAND_HUB_TASK_ */