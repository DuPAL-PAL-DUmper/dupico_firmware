#include "generic_ic_handler.h"
#include <tasks/ic_interfacer_task.h>
#include "basic_handlers_io.h"

#include <utils/custom_debug.h>

typedef const cmnd_list_entry* (*cmd_list_getter)(uint*); // Parameter is pointing to a value that is going to be set to the length of the list

enum Command_List {
    CMD_WR_I,
    CMD_WR_IO,
    CMD_RD_IO,
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
        .name = "Write Controls",
        .id = CMD_WR_CTRL
    },
    {
        .name = "Write Power",
        .id = CMD_WR_PWR
    }
};

static void executor(cmnd_list_entry *cmnd, const IC_Ctrl_Struct *ic_ctrl, const ic_interfacer_task_params *interfacer_params, const void* param) {
    D_PRINTF("Got command %u with name %s\n", cmnd->id, cmnd->name);

    switch(cmnd->id) {
        case CMD_WR_I:
            handler_write_inputs(ic_ctrl, interfacer_params, *(uint32_t*)param);
            break;
        case CMD_WR_IO:
            handler_write_io(ic_ctrl, interfacer_params, *(uint16_t*)param);
            break;
        case CMD_RD_IO:
            *(uint16_t*)param = handler_read_io(ic_ctrl, interfacer_params);
            break;
        case CMD_WR_CTRL:
            handler_write_control(ic_ctrl, interfacer_params, *(uint8_t*)param);
            break;
        case CMD_WR_PWR:
            handler_write_power(ic_ctrl, interfacer_params, *(uint8_t*)param);
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