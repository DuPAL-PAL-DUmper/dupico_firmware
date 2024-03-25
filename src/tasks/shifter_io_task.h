#ifndef _SHIFTER_IO_TASK_
#define _SHIFTER_IO_TASK_

#include <stdint.h>

typedef enum {
    READ,
    WRITE
} shf_io_cmd_type;

typedef struct {
    shf_io_cmd_type cmd;
    uint64_t param;
} shf_io_cmd;

void shifter_io_task(void *params);

#endif /*_SHIFTER_IO_TASK_*/