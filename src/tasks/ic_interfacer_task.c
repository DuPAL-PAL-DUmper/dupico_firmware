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

static void send_response(QueueHandle_t resp_queue, ic_interfacer_command_response_type resp, uint id, uint32_t data);

static void dummy_command(QueueHandle_t resp_queue, uint id, interfacer_state *state, const void *params);
static void define_ic_command(QueueHandle_t resp_queue, uint id, interfacer_state *state, const void *params);
static void set_i_command(QueueHandle_t resp_queue, uint id, interfacer_state *state, const void *params);
static void get_i_command(QueueHandle_t resp_queue, uint id, interfacer_state *state, const void *params);
static void set_io_command(QueueHandle_t resp_queue, uint id, interfacer_state *state, const void *params);
static void get_io_command(QueueHandle_t resp_queue, uint id, interfacer_state *state, const void *params);
static void set_pwr_command(QueueHandle_t resp_queue, uint id, interfacer_state *state, const void *params);
static void get_pwr_command(QueueHandle_t resp_queue, uint id, interfacer_state *state, const void *params);
static void set_ctrl_command(QueueHandle_t resp_queue, uint id, interfacer_state *state, const void *params);
static void get_ctrl_command(QueueHandle_t resp_queue, uint id, interfacer_state *state, const void *params);

static cmd_func command_map[] = {
    define_ic_command, // DEFINE_IC
    set_i_command, // SET_I 
    get_i_command, // GET_I
    set_io_command, // SET_IO
    get_io_command, // GET_IO
    set_pwr_command, // SET_PWR
    get_pwr_command, // GET_PWR
    set_ctrl_command, // SET_CTRL
    get_ctrl_command // GET_CTRL
};

static void send_response(QueueHandle_t resp_queue, ic_interfacer_command_response_type resp, uint id, uint32_t data) {
    ic_interfacer_command_response rsp = {
        .response = resp,
        .id = id,
        .data = data
    };
    xQueueSend(resp_queue, (void*)&rsp, portMAX_DELAY);    
}

static void dummy_command(QueueHandle_t resp_queue, uint id, interfacer_state *state, const void *params) {
    D_PRINTF("Executing command with id %u\n", id);
    return;
}

static void define_ic_command(QueueHandle_t resp_queue, uint id, interfacer_state *state, const void *params) {
    D_PRINTF("Executing command with id %u\n", id);

    IC_Ctrl_Struct *param_data = (IC_Ctrl_Struct*)params;
    uint ctrl_struct_size = sizeof(IC_Ctrl_Struct);
    D_PRINTF("Defining new ic %s with data size %u\n", param_data->name, ctrl_struct_size);

    memset(state, 0, sizeof(interfacer_state));
    memcpy(&(state->cur_ic), param_data, ctrl_struct_size);

    send_response(resp_queue, CMD_OK, id, 0);
}

static void set_i_command(QueueHandle_t resp_queue, uint id, interfacer_state *state, const void *params) {
    D_PRINTF("Executing command with id %u\n", id);
    uint32_t data = *(uint32_t*)params;

    state->i_w = data;
    
    send_response(resp_queue, CMD_OK, id, data);
}

static void get_i_command(QueueHandle_t resp_queue, uint id, interfacer_state *state, const void *params) {
    D_PRINTF("Executing command with id %u\n", id);
    uint32_t data = ctrl_struct_mask_to_i(&(state->cur_ic), state->data);
    
    send_response(resp_queue, CMD_OK, id, data);
}

void set_io_command(QueueHandle_t resp_queue, uint id, interfacer_state *state, const void *params) {
    D_PRINTF("Executing command with id %u\n", id);
    uint16_t data = *(uint16_t*)params;

    state->io_w = data;
    
    send_response(resp_queue, CMD_OK, id, data);
}

void get_io_command(QueueHandle_t resp_queue, uint id, interfacer_state *state, const void *params) {
    D_PRINTF("Executing command with id %u\n", id);
    uint16_t data = ctrl_struct_mask_to_io(&(state->cur_ic), state->data);
    
    send_response(resp_queue, CMD_OK, id, data);
}

void set_pwr_command(QueueHandle_t resp_queue, uint id, interfacer_state *state, const void *params) {
    D_PRINTF("Executing command with id %u\n", id);
    uint8_t data = *(uint8_t*)params;

    state->pwr_w = data;
    
    send_response(resp_queue, CMD_OK, id, data);
}

void get_pwr_command(QueueHandle_t resp_queue, uint id, interfacer_state *state, const void *params) {
    D_PRINTF("Executing command with id %u\n", id);
    uint8_t data = ctrl_struct_mask_to_pwr(&(state->cur_ic), state->data);
    
    send_response(resp_queue, CMD_OK, id, data);
}

static void set_ctrl_command(QueueHandle_t resp_queue, uint id, interfacer_state *state, const void *params) {
    D_PRINTF("Executing command with id %u\n", id);
    uint8_t data = *(uint8_t*)params;

    state->ctrl_w = data;
    
    send_response(resp_queue, CMD_OK, id, data);   
}

static void get_ctrl_command(QueueHandle_t resp_queue, uint id, interfacer_state *state, const void *params) {
    D_PRINTF("Executing command with id %u\n", id);
    uint8_t data = ctrl_struct_mask_to_ctrl(&(state->cur_ic), state->data);
    
    send_response(resp_queue, CMD_OK, id, data);    
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
                case COMMIT:
                    D_PRINTF("Received COMMIT command with id %u\n", cmd.id);
                    uint64_t data_mask = 0;
                    data_mask |= ctrl_struct_i_to_mask(&(state.cur_ic), state.i_w);
                    data_mask |= ctrl_struct_io_to_mask(&(state.cur_ic), state.io_w);
                    data_mask |= ctrl_struct_pwr_to_mask(&(state.cur_ic), state.pwr_w);
                    data_mask |= ctrl_struct_ctrl_to_mask(&(state.cur_ic), state.ctrl_w);

                    // TODO: write the data mask to the SIPO and read out the result from the PISO
                    break;
                case DIE:
                    D_PRINTF("Received DIE command with id %u\n", cmd.id);
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