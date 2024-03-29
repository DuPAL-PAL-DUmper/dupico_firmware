#include "ic_interfacer_task.h"

#include "data_structs.h"

#include <utils/custom_debug.h>

#include <string.h>

typedef struct {
    IC_Ctrl_Struct *cur_ic;
} interfacer_state;

typedef void(*cmd_func)(QueueHandle_t, uint id, interfacer_state*, const void*);

void dummy_command(QueueHandle_t resp_queue, uint id, interfacer_state *state, const void *params);
void define_ic_command(QueueHandle_t resp_queue, uint id, interfacer_state *state, const void *params);

static cmd_func command_map[] = {
    define_ic_command, // DEFINE_IC
    dummy_command, // SET_ADDRESS
    dummy_command, // GET_ADDRESS 
    dummy_command, // SET_DATA
    dummy_command, // GET_DATA
    dummy_command, // SET_SPECIAL
    dummy_command, // GET_SPECIAL
};

void dummy_command(QueueHandle_t resp_queue, uint id, interfacer_state *state, const void *params) {
    return;
}

void define_ic_command(QueueHandle_t resp_queue, uint id, interfacer_state *state, const void *params) {
    D_PRINTF("Executing command with id %u\n", id);
    if(state->cur_ic) {
        D_PRINTF("Freeing previous definition %s\n", state->cur_ic->name);
        vPortFree(state->cur_ic);
    }
    IC_Ctrl_Struct *param_data = (IC_Ctrl_Struct*)params;
    uint ctrl_struct_size = sizeof(IC_Ctrl_Struct);
    D_PRINTF("Defining new ic %s with data size %u\n", param_data->name, ctrl_struct_size);

    state->cur_ic = pvPortMalloc(ctrl_struct_size);
    memcpy(state->cur_ic, param_data, ctrl_struct_size);

    ic_interfacer_command_response rsp = {
        .response = CMD_OK,
        .id = id
    };
    xQueueSend(resp_queue, (void*)&rsp, portMAX_DELAY);
}

void ic_interfacer_task(void *params) {
    interfacer_state state = {
        .cur_ic = NULL
    };

    ic_interfacer_task_params *prms = (ic_interfacer_task_params*)params;
    ic_interfacer_command cmd;
    bool keep_going = true;

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

    if(state.cur_ic) vPortFree(state.cur_ic);

    vTaskDelete(NULL);
}