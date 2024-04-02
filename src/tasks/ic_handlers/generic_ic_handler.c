#include "generic_ic_handler.h"
#include <tasks/ic_interfacer_task.h>

#include <utils/custom_debug.h>

typedef const cmnd_list_entry* (*cmd_list_getter)(uint*); // Parameter is pointing to a value that is going to be set to the length of the list

enum Command_List {
    CMD_WR_I,
    CMD_WR_IO,
    CMD_RD_IO,
    CMD_RD_O,
    CMD_WR_CTRL,
    CMD_WR_PWR
};

static cmnd_list_entry cmnd_entries[] = {
    {
        .name = "Write Inputs",
        .id = CMD_WR_I
    },
    {
        .name = "Write I/Os",
        .id = CMD_WR_IO
    },
    {
        .name = "Read I/Os",
        .id = CMD_RD_IO
    },
    {
        .name = "Read Outputs",
        .id = CMD_RD_O
    },
    {
        .name = "Write Controls",
        .id = CMD_WR_CTRL
    },
    {
        .name = "Write Power",
        .id = CMD_WR_CTRL
    }
};

static void executor(cmnd_list_entry *cmnd, const IC_Ctrl_Struct *ic_ctrl, const ic_interfacer_task_params *interfacer_params, const void* param) {
    D_PRINTF("Got command %u with name %s\n", cmnd->id, cmnd->name);

    switch(cmnd->id){
        default:
            return;
    }
}

static const cmnd_list_entry* get_cmd_list(uint* len) {
    *len = sizeof(cmnd_entries)/sizeof(cmnd_list_entry);
    return cmnd_entries;
}

const handler_funcs build_generic_ic_handler() {
    return (handler_funcs){
        .exec_command = executor,
        .get_commands = get_cmd_list
    };
}