#include "ic_interfacer_task.h"

#include "data_structs.h"

#include <utils/custom_debug.h>

#include <string.h>

typedef struct {
    IC_Ctrl_Struct cur_ic;
    uint32_t i_w;
    uint16_t io_w;
    uint8_t pwr_w;
    uint8_t ctrl_w;
    uint64_t data;
} interfacer_state;

typedef void(*cmd_func)(QueueHandle_t, uint id, interfacer_state*, const void*);

void dummy_command(QueueHandle_t resp_queue, uint id, interfacer_state *state, const void *params);
void define_ic_command(QueueHandle_t resp_queue, uint id, interfacer_state *state, const void *params);
void set_i_command(QueueHandle_t resp_queue, uint id, interfacer_state *state, const void *params);
void get_i_command(QueueHandle_t resp_queue, uint id, interfacer_state *state, const void *params);
void set_io_command(QueueHandle_t resp_queue, uint id, interfacer_state *state, const void *params);

static cmd_func command_map[] = {
    define_ic_command, // DEFINE_IC
    set_i_command, // SET_I 
    get_i_command, // GET_I
    set_io_command, // SET_IO
    dummy_command, 
    dummy_command, 
    dummy_command, 
    dummy_command, 
    dummy_command, 
};

void dummy_command(QueueHandle_t resp_queue, uint id, interfacer_state *state, const void *params) {
    return;
}

void define_ic_command(QueueHandle_t resp_queue, uint id, interfacer_state *state, const void *params) {
    D_PRINTF("Executing command with id %u\n", id);

    IC_Ctrl_Struct *param_data = (IC_Ctrl_Struct*)params;
    uint ctrl_struct_size = sizeof(IC_Ctrl_Struct);
    D_PRINTF("Defining new ic %s with data size %u\n", param_data->name, ctrl_struct_size);

    memset(state, 0, sizeof(interfacer_state));
    memcpy(&(state->cur_ic), param_data, ctrl_struct_size);

    ic_interfacer_command_response rsp = {
        .response = CMD_OK,
        .id = id
    };
    xQueueSend(resp_queue, (void*)&rsp, portMAX_DELAY);
}

void set_i_command(QueueHandle_t resp_queue, uint id, interfacer_state *state, const void *params) {
    D_PRINTF("Executing command with id %u\n", id);
    uint32_t data = *(uint32_t*)params;

    state->i_w = data;

    ic_interfacer_command_response rsp = {
        .response = CMD_OK,
        .id = id
    };
    xQueueSend(resp_queue, (void*)&rsp, portMAX_DELAY);
}

void get_i_command(QueueHandle_t resp_queue, uint id, interfacer_state *state, const void *params) {
    D_PRINTF("Executing command with id %u\n", id);
    uint32_t data = ctrl_struct_mask_to_i(&(state->cur_ic), state->data);

    ic_interfacer_command_response rsp = {
        .response = CMD_OK,
        .id = id,
        .data = data
    };

    xQueueSend(resp_queue, (void*)&rsp, portMAX_DELAY);    
}

void set_io_command(QueueHandle_t resp_queue, uint id, interfacer_state *state, const void *params) {
    D_PRINTF("Executing command with id %u\n", id);
    uint16_t data = *(uint16_t*)params;

    state->io_w = data;

    ic_interfacer_command_response rsp = {
        .response = CMD_OK,
        .id = id
    };
    xQueueSend(resp_queue, (void*)&rsp, portMAX_DELAY);
}

void ic_interfacer_task(void *params) {
    interfacer_state state;

    ic_interfacer_task_params *prms = (ic_interfacer_task_params*)params;
    ic_interfacer_command cmd;
    bool keep_going = true;

    memset(&state, 0, sizeof(interfacer_state));

    while(keep_going) {
        if(xQueueReceive(prms->cmd_queue, (void*)&cmd, portMAX_DELAY)) {
            switch(cmd.cmd) {
                case DIE:
                    ic_interfacer_command_response rsp = {
                        .response = CMD_OK,
                        .id = cmd.id
                    };
                    keep_going = false;
                    xQueueSend(prms->resp_queue, (void*)&rsp, portMAX_DELAY);
                    break;
                default:
                    command_map[cmd.cmd](prms->resp_queue, cmd.id, &state, cmd.param);
                    break;
            }
        }
    }

    vTaskDelete(NULL);
}