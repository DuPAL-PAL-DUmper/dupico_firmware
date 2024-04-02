#ifndef _IC_HANDLERS_HEADER_
#define _IC_HANDLERS_HEADER_

#include <stdint.h>
#include <pico/types.h>

#include <tasks/data_structs.h>
#include <tasks/ic_interfacer_task.h>

#define CMD_ENTRY_NAME_LEN 10
typedef struct {
    char name[CMD_ENTRY_NAME_LEN];
    uint id;
} cmnd_list_entry;

typedef void* (*cmd_executor)(uint, const IC_Ctrl_Struct*, const ic_interfacer_task_params *interfacer_params, const void*);
typedef const cmnd_list_entry* (*cmd_list_getter)(uint*); // Parameter is pointing to a value that is going to be set to the length of the list

typedef struct {
    const cmd_executor exec_command;
    const cmd_list_getter get_commands;
} handler_funcs;
typedef const handler_funcs (*handlers_getter)(void);

const handler_funcs get_handlers_for_IC_type(uint16_t ic_type);

#endif /* _IC_HANDLERS_HEADER_ */