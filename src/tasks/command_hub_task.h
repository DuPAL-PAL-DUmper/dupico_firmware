#ifndef _COMMAND_HUB_TASK_
#define _COMMAND_HUB_TASK_

#include <FreeRTOS.h>
#include <queue.h>
typedef enum {
    CMDH_RESET,
    CMDH_FORCE_ERROR,
    CMDH_WRITE_PINS,
    CMDH_READ_PINS,
    CMDH_TOGGLE_POWER
} command_hub_cmd_type;

typedef enum {
    CMDH_RESP_OK,
    CMDH_RESP_ERROR
} command_hub_cmd_response_type;

typedef union {
    uint64_t data;
} command_hub_cmd_resp_data;

typedef struct {
    command_hub_cmd_type type;
    uint32_t id;
    uint64_t data;
} command_hub_cmd;

typedef struct {
    uint32_t id;
    command_hub_cmd_type cmd_type;
    command_hub_cmd_response_type type;
    command_hub_cmd_resp_data data;
} command_hub_cmd_resp;

typedef struct {
    QueueHandle_t cmd_queue;
    QueueHandle_t resp_queue;
} command_hub_queues;

#define CMD_QUEUE_SIZE 14

void command_hub_task(void *params);

#endif /* _COMMAND_HUB_TASK_ */