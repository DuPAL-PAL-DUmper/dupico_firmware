#include "ic_interfacer_task.h"

#include <pico/platform.h>
#include <string.h>

#include "data_structs.h"
#include <tasks/shifter_io_task.h>
#include <utils/common_macros.h>
#include <utils/custom_debug.h>


typedef struct {
    IC_Ctrl_Struct cur_ic;
    uint32_t i_w;
    uint16_t io_w;
    uint8_t pwr_w;
    uint8_t ctrl_w;
    uint64_t data;
} interfacer_state;

typedef void(*cmd_func)(QueueHandle_t, uint, interfacer_state*, const void*);

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

static cmd_func __in_flash() command_map[] = {
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
    D_PRINTF("Defining new ic %s\n\tType %.4X [%s]\n\tI length %u\n\tIO length %u\n\tPWR length %u\n\tCTRL length %u\n",
        param_data->name,
        param_data->chip_type,
        get_ic_type_name(param_data->chip_type),
        param_data->i_len,
        param_data->io_len,
        param_data->pwr_len,
        param_data->ctrl_len);

    memset(state, 0, sizeof(interfacer_state));
    memcpy(&(state->cur_ic), param_data, ctrl_struct_size);

    send_response(resp_queue, CMD_OK, id, 0);
}

static void set_i_command(QueueHandle_t resp_queue, uint id, interfacer_state *state, const void *params) {
    uint32_t data = *(uint32_t*)params;
    D_PRINTF("Executing command with id %u and param %u\n", id, data);

    state->i_w = data;
    
    send_response(resp_queue, CMD_OK, id, data);
}

static void get_i_command(QueueHandle_t resp_queue, uint id, interfacer_state *state, const void *params) {
    D_PRINTF("Executing command with id %u\n", id);
    uint32_t data = ctrl_struct_mask_to_i(&(state->cur_ic), state->data);
    
    send_response(resp_queue, CMD_OK, id, data);
}

void set_io_command(QueueHandle_t resp_queue, uint id, interfacer_state *state, const void *params) {
    uint16_t data = *(uint16_t*)params;
    D_PRINTF("Executing command with id %u and param %u\n", id, data);

    state->io_w = data;
    
    send_response(resp_queue, CMD_OK, id, data);
}

void get_io_command(QueueHandle_t resp_queue, uint id, interfacer_state *state, const void *params) {
    D_PRINTF("Executing command with id %u\n", id);
    uint16_t data = ctrl_struct_mask_to_io(&(state->cur_ic), state->data);
    
    send_response(resp_queue, CMD_OK, id, data);
}

void set_pwr_command(QueueHandle_t resp_queue, uint id, interfacer_state *state, const void *params) {
    uint8_t data = *(uint8_t*)params;
    D_PRINTF("Executing command with id %u and param %u\n", id, data);

    state->pwr_w = data;
    
    send_response(resp_queue, CMD_OK, id, data);
}

void get_pwr_command(QueueHandle_t resp_queue, uint id, interfacer_state *state, const void *params) {
    D_PRINTF("Executing command with id %u\n", id);
    uint8_t data = ctrl_struct_mask_to_pwr(&(state->cur_ic), state->data);
    
    send_response(resp_queue, CMD_OK, id, data);
}

static void set_ctrl_command(QueueHandle_t resp_queue, uint id, interfacer_state *state, const void *params) {
    uint8_t data = *(uint8_t*)params;
    D_PRINTF("Executing command with id %u and param %u\n", id, data);

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

    TaskHandle_t shf_task_handle;
    shifter_io_task_params shf_prms = {
        .piso_cfg = {
            .ce_pin = 28,
            .pe_pin = 27,
            .clk_pin = 26,
            .clr_pin = 22,
            .ser_pin = 21,
            .len = 64
        },
        .sipo_cfg = {
            .oe_pin = 20,
            .ser_pin = 19,
            .srclk_pin = 18,
            .rclk_pin = 17,
            .srclr_pin = 16,
            .len = 64
        },
        .cmd_queue = xQueueCreate(1, sizeof(shifter_io_task_cmd)),
        .resp_queue = xQueueCreate(1, sizeof(uint64_t))
    };

    // Initialization
    xTaskCreate(shifter_io_task, "Shifter_IO_Task", configMINIMAL_STACK_SIZE, (void*)&shf_prms, BASELINE_TASK_PRIORITY, &shf_task_handle);
    memset(&state, 0, sizeof(interfacer_state));

    // Task loop
    while(keep_going) {
        D_PRINTF("IC Interfacer task loop\n");
        
        if(xQueueReceive(prms->cmd_queue, (void*)&cmd, portMAX_DELAY)) {
            switch(cmd.cmd) {
                case INTF_COMMIT:
                    D_PRINTF("Received COMMIT command with id %u\n", cmd.id);
                    uint64_t data_mask = 0;
                    data_mask |= ctrl_struct_i_to_mask(&(state.cur_ic), state.i_w);
                    data_mask |= ctrl_struct_io_to_mask(&(state.cur_ic), state.io_w);
                    data_mask |= ctrl_struct_pwr_to_mask(&(state.cur_ic), state.pwr_w);
                    data_mask |= ctrl_struct_ctrl_to_mask(&(state.cur_ic), state.ctrl_w);

                    // Write the data mask to the SIPO and read out the result from the PISO
                    shifter_io_task_cmd shf_cmd_wr = {
                        .cmd = SHF_WRITE,
                        .param = data_mask
                    };
                    xQueueSend(shf_prms.cmd_queue, (void*)&shf_cmd_wr, portMAX_DELAY);

                    // Relay back the response
                    if(xQueueReceive(shf_prms.resp_queue, (void*)&(state.data), portMAX_DELAY)) {
                        send_response(prms->resp_queue, CMD_OK, cmd.id, state.data);    
                    } else {
                        D_PRINTF("Unable to read from queue!!!\n");
                        send_response(prms->resp_queue, CMD_KO, cmd.id, 0);    
                    }

                    break;
                case INTF_DIE:
                    D_PRINTF("Received DIE command with id %u\n", cmd.id);
                    // Ask the other task to die
                    shifter_io_task_cmd shf_cmd_die = {
                        .cmd = SHF_DIE,
                    };
                    xQueueSend(shf_prms.cmd_queue, (void*)&shf_cmd_die, portMAX_DELAY);

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