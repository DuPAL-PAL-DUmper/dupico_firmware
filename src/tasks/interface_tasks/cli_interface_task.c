#include "cli_interface_task.h"

#include <string.h>
#include <stdint.h>

#include <FreeRTOS.h>
#include <queue.h>
#include <task.h>

#include <pico/platform.h>
#include <pico/stdio_usb.h>

#include "cmd_handlers/cmd_handler.h"
#include "cmd_handlers/bin_cmd_handler.h"

#include "tasks/command_hub_task.h"
#include "utils/custom_debug.h"
#include "utils/strutils.h"

#define CMD_BUFFER_SIZE 64
static char cmd_buffer[CMD_BUFFER_SIZE] __attribute__((aligned(4)));

const handler_funcs __in_flash() HANDLER_MAP[] = {
    {
        .c_handler = bin_cmd_handler,
        .cr_handler = bin_response_handler
    }
};

static bool cli_test_mode(command_hub_queues *queues);
static void cli_request_reset(command_hub_queues *queues);
static void cli_handle_responses(char cmd_buffer[CMD_BUFFER_SIZE], command_hub_queues *queues);

static void cli_request_reset(command_hub_queues *queues) {
    command_hub_cmd_resp cmdh_resp;

    // Tell the command hub to reset
    xQueueSend(queues->cmd_queue, (void*)& ((command_hub_cmd){
        .type = CMDH_RESET,
        .data = 0
    }), portMAX_DELAY);

    if(!xQueueReceive(queues->resp_queue, (void*)&(cmdh_resp), portMAX_DELAY)) {
        D_PRINTF("Error requesting a reset from the command hub\r\n");
    } else {
        DD_PRINTF("Reset from command hub responded with %u - %u\r\n", cmdh_resp.id, cmdh_resp.type);
    }    
}

void cli_interface_task(void *params) {
    uint8_t handler_num = 0; // We start in the classic text handler mode
    uint16_t cmd_handler_response;
    
    command_hub_queues *queues = (command_hub_queues*)params;

    bool term_connected_state = false;
    bool cur_term_connected = false;
    int ch;

    handler_config cmd_handler_config = {
        .buf_idx = 0,
        .buf_size = CMD_BUFFER_SIZE,
        .cmd_buffer = cmd_buffer,
        .queues = queues,
        .test_func = cli_test_mode
    };

    UBaseType_t cmd_queue_len = uxQueueGetQueueLength(queues->cmd_queue);

    while(true) {
        cur_term_connected = stdio_usb_connected();

        if(cur_term_connected != term_connected_state) {
            term_connected_state = cur_term_connected;

            // Terminal state changed, requesting a reset to the hub
            cli_request_reset(queues);

            if(!term_connected_state) { // We got a disconnection from the device
                D_PRINTF("Serial terminal disconnected!\r\n");
            } else { // New connection!
                D_PRINTF("Serial terminal connected!\r\n");

                // Reset command handler to default text based commands
                cmd_handler_config.buf_idx = 0;
                handler_num = 0;

                USB_PRINTF("REMOTE_CONTROL_ENABLED\r\n");
            }
        }

        if(term_connected_state) {
            int count = 0;
            while((cmd_queue_len > uxQueueMessagesWaiting(queues->cmd_queue)) &&
                ((count = stdio_usb.in_chars(cmd_handler_config.cmd_buffer + cmd_handler_config.buf_idx, cmd_handler_config.buf_size - cmd_handler_config.buf_idx)) > 0)) {
                cmd_handler_config.buf_idx += count;
                cmd_handler_response = HANDLER_MAP[handler_num].c_handler(&cmd_handler_config);
            }
        }

        HANDLER_MAP[handler_num].cr_handler(&cmd_handler_config);

        // Check if we got a request to switch protocol
        if((cmd_handler_response & 0xFF00) == CMD_HANDLER_SWITCH_PROTO) {
            uint8_t req_proto = cmd_handler_response & 0xFF;
            D_PRINTF("Switch to protocol %.2X requested.\r\n", req_proto);
            cmd_handler_response = CMD_HANDLER_NO_CMD;

            cmd_handler_config.buf_idx = 0;

            if(req_proto >= (sizeof(HANDLER_MAP)/sizeof(handler_funcs))) {
                D_PRINTF("Protocol %.2X not supported, switching back to text mode.\r\n", req_proto);
                req_proto = 0;
            }
            
            // Switch the protocol
            handler_num = req_proto;
        }
        
        taskYIELD();
    } 
}

static bool cli_test_mode(command_hub_queues *queues) {
    command_hub_cmd_resp cmdh_resp;
    uint64_t test_patterns[] = {0x0000000000ULL, 0xFFFFFFFFFFULL, 0xAAAAAAAAAAULL, 0x5555555555ULL, 0x123456789AULL, 0xA987654321ULL};
    uint8_t tot_patterns = sizeof(test_patterns) / sizeof(test_patterns[0]);
    bool test_result = true;

    DD_PRINTF("Executing test mode...\r\n");

    // Enable the relay
    DD_PRINTF("Enabling the relay.\r\n");
    xQueueSend(queues->cmd_queue, (void*)& ((command_hub_cmd){
        .type = CMDH_TOGGLE_POWER,
        .data = 1
    }), portMAX_DELAY);
    xQueueReceive(queues->resp_queue, (void*)&(cmdh_resp), portMAX_DELAY);

    vTaskDelay(100); // Give some time for the shifter outputs to stabilize

    for(uint8_t idx = 0; idx < tot_patterns; idx++) {
        DD_PRINTF("Testing %.16X.\r\n", test_patterns[idx]);
        xQueueSend(queues->cmd_queue, (void*)& ((command_hub_cmd){
            .type = CMDH_WRITE_PINS,
            .data = test_patterns[idx]
        }), portMAX_DELAY);
        xQueueReceive(queues->resp_queue, (void*)&(cmdh_resp), portMAX_DELAY);

        if(cmdh_resp.data.data != test_patterns[idx]) {
            D_PRINTF("Failed pattern %llx, got %llx!\r\n", test_patterns[idx], cmdh_resp.data.data);

            test_result = false;
            break;
        }
    }
    
    vTaskDelay(100);

    // Disable the relay
    DD_PRINTF("Disabling the relay.\r\n");
    xQueueSend(queues->cmd_queue, (void*)& ((command_hub_cmd){
        .type = CMDH_TOGGLE_POWER,
        .data = 0
    }), portMAX_DELAY);
    xQueueReceive(queues->resp_queue, (void*)&(cmdh_resp), portMAX_DELAY);

    return test_result;
}