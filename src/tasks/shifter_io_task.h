#ifndef _SHIFTER_IO_TASK_
#define _SHIFTER_IO_TASK_

#include <stdint.h>

#include <hardware/shifters/piso_shifter.h>
#include <hardware/shifters/sipo_shifter.h>

#include <queue.h>

typedef enum {
    READ,
    WRITE,
    DIE
} shifter_io_task_cmd_type;

typedef struct {
    shifter_io_task_cmd_type cmd;
    uint64_t param;
} shifter_io_task_cmd;

typedef struct {
    PISO_Config piso_cfg;
    SIPO_Config sipo_cfg;

    QueueHandle_t cmd_queue;
    QueueHandle_t resp_queue;
} shifter_io_task_params;

void shifter_io_task(void *params);

#endif /*_SHIFTER_IO_TASK_*/