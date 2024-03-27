#ifndef _CMD_EXECUTOR_TASK_
#define _CMD_EXECUTOR_TASK_

#include <FreeRTOS.h>
#include <queue.h>

typedef enum {
    DEFINE_IC = 0,
    SET_ADDRESS = 1,
    GET_ADDRESS = 2,
    SET_DATA = 3,
    GET_DATA = 4,
    SET_SPECIAL = 5,
    GET_SPECIAL = 6,
    DIE
} cmd_executor_command_type;

typedef struct {
    const cmd_executor_command_type cmd;
    const uint id;
    const void *param;
} cmd_executor_command;

typedef struct {
    const QueueHandle_t cmd_queue;
    const QueueHandle_t resp_queue;
} cmd_executor_task_params;

void cmd_executor_task(void *params);

#endif /* _CMD_EXECUTOR_TASK_ */