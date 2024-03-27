#include "ic_interfacer_task.h"

#include "data_structs.h"

#include <string.h>

typedef struct {
    IC_Control_Data *cur_ic;
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
    if(state->cur_ic) vPortFree(state->cur_ic);
    IC_Control_Data *param_data = (IC_Control_Data*)params;
    uint ctrl_data_size = calculate_IC_Control_Data_size(param_data);

    state->cur_ic = pvPortMalloc(ctrl_data_size);
    memcpy(state->cur_ic, param_data, ctrl_data_size);

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