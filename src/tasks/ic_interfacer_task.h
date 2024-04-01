#ifndef _CMD_EXECUTOR_TASK_
#define _CMD_EXECUTOR_TASK_

#include <FreeRTOS.h>
#include <queue.h>

typedef enum {
    INTF_DEFINE_IC = 0,
    INTF_SET_I = 1,
    INTF_GET_I = 2,
    INTF_SET_IO = 3,
    INTF_GET_IO = 4,
    INTF_SET_PWR = 5,
    INTF_GET_PWR = 6,
    INTF_SET_CTRL = 7,
    INTF_GET_CTRL = 8,
    INTF_COMMIT,
    INTF_DIE
} ic_interfacer_command_type;

typedef enum {
    CMD_OK = 0,
    CMD_KO
} ic_interfacer_command_response_type;

typedef struct {
    const ic_interfacer_command_response_type response;
    const uint id;
    const uint32_t data;
} ic_interfacer_command_response;

typedef struct {
    const ic_interfacer_command_type cmd;
    const uint id;
    const void *param;
} ic_interfacer_command;

typedef struct {
    const QueueHandle_t cmd_queue;
    const QueueHandle_t resp_queue;
} ic_interfacer_task_params;

void ic_interfacer_task(void *params);

#endif /* _CMD_EXECUTOR_TASK_ */