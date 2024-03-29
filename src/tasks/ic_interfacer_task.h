#ifndef _CMD_EXECUTOR_TASK_
#define _CMD_EXECUTOR_TASK_

#include <FreeRTOS.h>
#include <queue.h>

typedef enum {
    DEFINE_IC = 0,
    SET_I = 1,
    GET_I = 2,
    SET_IO = 3,
    GET_IO = 4,
    SET_PWR = 5,
    GET_PWR = 6,
    SET_CTRL = 7,
    GET_CTRL = 8,
    COMMIT = 9,
    DIE
} ic_interfacer_command_type;

typedef enum {
    CMD_OK,
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