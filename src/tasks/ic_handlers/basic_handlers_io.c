#include "basic_handlers_io.h"

#include <utils/custom_debug.h>

static uint cmd_counter = 0;

static uint32_t send_and_check_command(ic_interfacer_command *cmd, const ic_interfacer_task_params *interfacer_params);

static uint32_t send_and_check_command(ic_interfacer_command *cmd, const ic_interfacer_task_params *interfacer_params) {
    ic_interfacer_command_response resp;

    xQueueSend(interfacer_params->cmd_queue, (void*)cmd, portMAX_DELAY);
    if(xQueueReceive(interfacer_params->resp_queue, (void*)&(resp), portMAX_DELAY)) { 
        D_PRINTF("Received response for command %u with status %u\n", resp.id, resp.response);
        if(resp.id != cmd->id) {
            D_PRINTF("Unexpected command response id! %u -> %u\n", cmd->id, resp.id);
        }

        return resp.data;
    }
}

void handler_set_inputs(const ic_interfacer_task_params *interfacer_params, uint32_t inputs) {
    ic_interfacer_command cmd = {
        .cmd = INTF_SET_I,
        .id = cmd_counter++,
        .param = (void*)&inputs
    };

    send_and_check_command(&cmd, interfacer_params);
}

void handler_set_io(const ic_interfacer_task_params *interfacer_params, uint16_t io) {
    ic_interfacer_command cmd = {
        .cmd = INTF_SET_IO,
        .id = cmd_counter++,
        .param = (void*)&io
    };

    send_and_check_command(&cmd, interfacer_params);
}

uint16_t handler_get_io(const ic_interfacer_task_params *interfacer_params) {
    ic_interfacer_command cmd = {
        .cmd = INTF_GET_IO,
        .id = cmd_counter++,
        .param = NULL
    };

    return send_and_check_command(&cmd, interfacer_params) & 0xFFFF;
}

void handler_set_power(const ic_interfacer_task_params *interfacer_params, uint8_t power) {
    ic_interfacer_command cmd = {
        .cmd = INTF_SET_PWR,
        .id = cmd_counter++,
        .param = (void*)&power
    };

    send_and_check_command(&cmd, interfacer_params);
}

void handler_set_control(const ic_interfacer_task_params *interfacer_params, uint8_t controls) {
    ic_interfacer_command cmd = {
        .cmd = INTF_SET_CTRL,
        .id = cmd_counter++,
        .param = (void*)&controls
    };

    send_and_check_command(&cmd, interfacer_params);
}

void handler_commit(const ic_interfacer_task_params *interfacer_params) {
    ic_interfacer_command cmd = {
        .cmd = INTF_COMMIT,
        .id = cmd_counter++,
        .param = NULL
    };

    send_and_check_command(&cmd, interfacer_params);
}