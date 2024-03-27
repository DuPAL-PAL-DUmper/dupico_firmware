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
} ic_interfacer_command_type;

typedef enum {
    CMD_OK,
    CMD_KO
} ic_interfacer_command_response_type;

typedef struct {
    const ic_interfacer_command_response_type response;
    const uint id;
    const uint64_t data;
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