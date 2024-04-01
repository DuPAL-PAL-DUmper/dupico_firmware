#ifndef _IC_HANDLERS_HEADER_
#define _IC_HANDLERS_HEADER_

#include <stdint.h>
#include <pico/types.h>

#include <tasks/data_structs.h>

#define CMD_ENTRY_NAME_LEN 10
typedef struct {
    char name[CMD_ENTRY_NAME_LEN];
    uint8_t id;
} cmnd_list_entry;

typedef void* (*cmd_executor)(uint, IC_Ctrl_Struct*, const void*);
typedef const cmnd_list_entry* (*cmd_list_getter)(uint*); // Parameter is pointing to a value that is going to be set to the length of the list

typedef struct {
    cmd_executor exec_command;
    cmd_list_getter get_commands;
} handler_funcs;

handler_funcs get_handlers_for_IC_type(uint16_t ic_type);

#endif /* _IC_HANDLERS_HEADER_ */