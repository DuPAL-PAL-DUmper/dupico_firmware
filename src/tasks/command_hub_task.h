#ifndef _COMMAND_HUB_TASK_
#define _COMMAND_HUB_TASK_

#include <FreeRTOS.h>
#include <queue.h>

typedef enum {
    CMDH_SUPPORTED_IC_COUNT,
    CMDH_SUPPORTED_IC_BEGIN_LIST,
    CMDH_SUPPORTED_IC_LIST_NEXT,
    CMDH_SUPPORTED_IC_LIST_PREV
} command_hub_cmd_type;

typedef struct {
    command_hub_cmd_type type;
    uint id;
} command_hub_cmd;

typedef struct {
    uint id;
} command_hub_cmd_resp;

typedef struct {
    QueueHandle_t cmd_queue;
    QueueHandle_t resp_queue;
    QueueHandle_t cmd_update_queue;
} command_hub_queues;

void command_hub_task(void *params);

#endif /* _COMMAND_HUB_TASK_ */