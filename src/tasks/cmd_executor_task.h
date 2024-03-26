#ifndef _CMD_EXECUTOR_TASK_
#define _CMD_EXECUTOR_TASK_

#include <FreeRTOS.h>
#include <queue.h>

typedef enum {
    READ = 0,
    WRITE = 1,
    GET_POWER = 2,
    SET_POWER = 3,
    DUMP_TO_SD = 4,
    STREAM_FROM_SD = 5,
    DIE
} cmd_executor_command_type;

typedef struct {
    const cmd_executor_command_type cmd;
    const void *param;
} cmd_executor_command;

typedef struct {
    const QueueHandle_t cmd_queue;
    const QueueHandle_t resp_queue;
} cmd_executor_task_params;

void cmd_executor_task(void *params);

#endif /* _CMD_EXECUTOR_TASK_ */