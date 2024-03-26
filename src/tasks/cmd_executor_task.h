#ifndef _CMD_EXECUTOR_TASK_
#define _CMD_EXECUTOR_TASK_

#include <FreeRTOS.h>
#include <queue.h>

typedef enum {
    READ_RAW = 0,
    WRITE_RAW = 1,
    DUMP_TO_SD = 2,
    STREAM_FROM_SD = 3,
    SRAM_CHECK = 4,
    DRAM_CHECK = 5,
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