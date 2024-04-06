#ifndef _IC_HANDLERS_HEADER_
#define _IC_HANDLERS_HEADER_

#include <stdint.h>
#include <pico/types.h>

#include <tasks/data_structs.h>
#include <tasks/ic_interfacer_task.h>

#define CMD_ENTRY_NAME_LEN 16

typedef enum {
    CMD_TYPE_CLI_ONLY,
    CMD_TYPE_STANDALONE_ONLY,
    CMD_TYPE_BOTH
} cmd_entry_type;

typedef struct {
    char name[CMD_ENTRY_NAME_LEN];
    uint id;
    cmd_entry_type type;
} cmd_list_entry;

typedef enum {
    CMD_STATUS_STARTING,
    CMD_STATUS_DONE,
    CMD_STATUS_FAILED,
    CMD_STATUS_PROGRESS
} cmd_status_types;

typedef struct {
    cmd_list_entry cmd;
    cmd_status_types status;
    uint64_t data;
} cmd_status_update;

typedef int8_t (*cmd_executor)(const cmd_list_entry*, const IC_Ctrl_Struct*, const ic_interfacer_task_params *interfacer_params, const QueueHandle_t update_queue, const void*);
typedef const cmd_list_entry* (*cmd_list_getter)(uint*); // Parameter is pointing to a value that is going to be set to the length of the list

typedef struct {
    const cmd_executor exec_command;
    const cmd_list_getter get_commands;
} handler_funcs;
typedef const handler_funcs (*handlers_getter)(void);

const handler_funcs get_handlers_for_IC_type(uint16_t ic_type);

#endif /* _IC_HANDLERS_HEADER_ */