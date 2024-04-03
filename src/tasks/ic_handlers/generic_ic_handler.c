#include "generic_ic_handler.h"
#include <tasks/ic_interfacer_task.h>
#include "basic_handlers_io.h"

#include <utils/custom_debug.h>

typedef const cmnd_list_entry* (*cmd_list_getter)(uint*); // Parameter is pointing to a value that is going to be set to the length of the list

enum Command_List {
    CMD_SET_I,
    CMD_SET_IO,
    CMD_GET_IO,
    CMD_SET_CTRL,
    CMD_SET_PWR,
    CMD_COMMIT
};

static cmnd_list_entry cmnd_entries[] = {
    {
        .name = "SET Inputs",
        .id = CMD_SET_I
    },
    {
        .name = "SET I/Os",
        .id = CMD_SET_IO
    },
    {
        .name = "GET I/Os",
        .id = CMD_GET_IO
    },
    {
        .name = "SET Controls",
        .id = CMD_SET_CTRL
    },
    {
        .name = "SET Power",
        .id = CMD_SET_PWR
    },
    {
        .name = "COMMIT",
        .id = CMD_COMMIT
    }
};

static void executor(cmnd_list_entry *cmnd, const IC_Ctrl_Struct *ic_ctrl, const ic_interfacer_task_params *interfacer_params, const void* param) {
    D_PRINTF("Got command %u with name %s\n", cmnd->id, cmnd->name);

    switch(cmnd->id) {
        case CMD_SET_I:
            handler_set_inputs(interfacer_params, *(uint32_t*)param);
            break;
        case CMD_SET_IO:
            handler_set_io(interfacer_params, *(uint16_t*)param);
            break;
        case CMD_GET_IO:
            *(uint16_t*)param = handler_get_io(interfacer_params);
            break;
        case CMD_SET_CTRL:
            handler_set_control(interfacer_params, *(uint8_t*)param);
            break;
        case CMD_SET_PWR:
            handler_set_power(interfacer_params, *(uint8_t*)param);
            break;
        case CMD_COMMIT:
            *(uint16_t*)param = handler_commit(interfacer_params);
            break;
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