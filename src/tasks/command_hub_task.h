#ifndef _COMMAND_HUB_TASK_
#define _COMMAND_HUB_TASK_

#include <tasks/data_structs.h>
#include <tasks/ic_handlers/ic_handlers.h>

#include <FreeRTOS.h>
#include <queue.h>

typedef enum {
    CMDH_SUPPORTED_IC_COUNT,
    CMDH_SUPPORTED_IC_BEGIN_LIST,
    CMDH_SUPPORTED_IC_LIST_NEXT,
    CMDH_SUPPORTED_IC_LIST_SELECT,
    CMDH_SELECTED_IC_GET_CMD_LIST,
    CMDH_SELECTED_IC_EXEC_CMD,
    CMDH_RESET
} command_hub_cmd_type;

typedef enum {
    CMDH_RESP_OK,
    CMDH_RESP_ERROR
} command_hub_cmd_response_type;

typedef struct {
    const cmd_list_entry *cmds;
    uint size;
} command_hub_cmd_resp_cmdlist;

typedef union {
    command_hub_cmd_resp_cmdlist cmdlist;
    cmd_list_entry cmd;
    IC_Ctrl_Struct iccd;
    uint32_t data;
} command_hub_cmd_resp_data;

typedef struct {
    command_hub_cmd_type type;
    uint id;
} command_hub_cmd;

typedef struct {
    uint id;
    command_hub_cmd_response_type type;
    command_hub_cmd_resp_data data;
} command_hub_cmd_resp;

typedef struct {
    QueueHandle_t cmd_queue;
    QueueHandle_t resp_queue;
    QueueHandle_t cmd_update_queue;
} command_hub_queues;

void command_hub_task(void *params);

#endif /* _COMMAND_HUB_TASK_ */