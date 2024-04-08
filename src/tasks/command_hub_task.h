#ifndef _COMMAND_HUB_TASK_
#define _COMMAND_HUB_TASK_

#include <FreeRTOS.h>

typedef struct {
} command_hub_command;

typedef struct {
} command_hub_command_response;

void command_hub_task(void *params);

#endif /* _COMMAND_HUB_TASK_ */