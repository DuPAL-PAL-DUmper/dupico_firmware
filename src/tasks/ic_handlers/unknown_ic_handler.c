#include "unknown_ic_handler.h"

typedef const cmnd_list_entry* (*cmd_list_getter)(uint*); // Parameter is pointing to a value that is going to be set to the length of the list

static cmnd_list_entry cmnd_entries[] = {};

static void* executor(uint id, IC_Ctrl_Struct *ic_ctrl, const void* param) {
    return NULL;
}

static cmnd_list_entry* get_cmd_list(uint* len) {
    *len = sizeof(cmnd_entries)/sizeof(cmnd_list_entry);
    return cmnd_entries;
}

const handler_funcs build_unknown_ic_handler() {
    return (handler_funcs){
        .exec_command = executor,
        .get_commands = get_cmd_list
    };
}