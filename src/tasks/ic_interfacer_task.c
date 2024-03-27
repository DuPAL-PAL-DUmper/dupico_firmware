#include "ic_interfacer_task.h"

#include "data_structs.h"

typedef struct {
    IC_Control_Data *cur_ic;
} interfacer_state;

typedef void(*cmd_func)(QueueHandle_t, interfacer_state*, const void*);

void dummy_command(QueueHandle_t resp_queue, interfacer_state *state, const void *params);
void define_ic_command(QueueHandle_t resp_queue, interfacer_state *state, const void *params);

static cmd_func command_map[] = {
    define_ic_command, // DEFINE_IC
    dummy_command, // SET_ADDRESS
    dummy_command, // GET_ADDRESS 
    dummy_command, // SET_DATA
    dummy_command, // GET_DATA
    dummy_command, // SET_SPECIAL
    dummy_command, // GET_SPECIAL
};

void dummy_command(QueueHandle_t resp_queue, interfacer_state *state, const void *params) {
    return;
}

void define_ic_command(QueueHandle_t resp_queue, interfacer_state *state, const void *params) {
    if(state->cur_ic) vPortFree(state->cur_ic);
    // TODO: copy over the ic definition data
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
                    keep_going = false;
                    break;
                default:
                    command_map[cmd.cmd](prms->resp_queue, &state, cmd.param);
                    break;
            }
        }
    }

    vTaskDelete(NULL);
}