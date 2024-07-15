#include "cli_interface_task.h"

#include <string.h>

#include <FreeRTOS.h>
#include <queue.h>
#include <task.h>

#include "pico/stdio_usb.h"

#include "tasks/command_hub_task.h"
#include "utils/custom_debug.h"
#include "utils/strutils.h"

#define VERSION "0.0.1"
#define SOFT_HEADER "\nDuPICO - " VERSION "\n\n\r"

#define MODEL "3"

#define DATA_PARAMETER_SIZE (1 + 16) // a whitespace plus hex representation of a 64bit number

#define CMD_BUFFER_SIZE 256
#define PKT_START '>'
#define PKT_END '<'
#define RESP_START '['
#define RESP_END ']'

#define CMD_WRITE 'W'
#define CMD_EXT_WRITE 'E'
#define CMD_READ 'R'
#define CMD_RESET 'K'
#define CMD_POWER 'P'
#define CMD_MODEL 'M'
#define CMD_TEST 'T'

#define RESP_ERROR "CMD_ERR\n\r"
#define RESP_MODEL "[M " MODEL "]\n\r"

static char cmd_buffer[CMD_BUFFER_SIZE];

static bool cli_test_mode(command_hub_queues *queues);
static void cli_parse_command(char cmd_buffer[CMD_BUFFER_SIZE], command_hub_queues *queues);
static void cli_request_reset(command_hub_queues *queues);
static void cli_handle_responses(char cmd_buffer[CMD_BUFFER_SIZE], command_hub_queues *queues);

static void cli_request_reset(command_hub_queues *queues) {
    command_hub_cmd_resp cmdh_resp;

    // Tell the command hub to reset
    xQueueSend(queues->cmd_queue, (void*)& ((command_hub_cmd){
        .type = CMDH_RESET,
        .data = 0,
        .id = 0
    }), portMAX_DELAY);

    if(!xQueueReceive(queues->resp_queue, (void*)&(cmdh_resp), portMAX_DELAY)) {
        D_PRINTF("Error requesting a reset from the command hub\r\n");
    } else {
        DD_PRINTF("Reset from command hub responded with %u - %u\r\n", cmdh_resp.id, cmdh_resp.type);
    }    
}

void cli_interface_task(void *params) {
    command_hub_queues *queues = (command_hub_queues*)params; 

    uint8_t buf_idx = 0;
    bool receiving_cmd = false;
    bool term_connected_state = false;
    bool cur_term_connected = false;
    int ch;

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

                buf_idx = 0;
                receiving_cmd = false;

                USB_PRINTF(SOFT_HEADER);
                USB_PRINTF("REMOTE_CONTROL_ENABLED\r\n");
            }
        }

        if(term_connected_state) {
            // If we have characters and space in the destination command queue, keep parsing...
            while(((ch = getchar_timeout_us(0)) >= 0) && (cmd_queue_len > uxQueueMessagesWaiting(queues->cmd_queue))) {
                switch(ch) {
                    case PKT_START:
                        memset(cmd_buffer, 0, CMD_BUFFER_SIZE);
                        buf_idx = 0;
                        receiving_cmd = true;
                        break;
                    case PKT_END:
                        if(receiving_cmd && buf_idx) {
                            // Parse and react to command
                            cli_parse_command(cmd_buffer, queues);
                        }
                        receiving_cmd = false;
                        buf_idx = 0;
                        break;
                    default:
                        if(receiving_cmd && (buf_idx < (CMD_BUFFER_SIZE - 1))) { // Leave one empty space for a null
                            cmd_buffer[buf_idx++] = ch & 0xFF;
                        }
                        break;
                }
            }
        }

        cli_handle_responses(cmd_buffer, queues);

        taskYIELD();
    } 
}

static void cli_handle_responses(char cmd_buffer[CMD_BUFFER_SIZE], command_hub_queues *queues) {
    command_hub_cmd_resp cmdh_resp;

    while(xQueueReceive(queues->resp_queue, (void*)&(cmdh_resp), 0)) {
        cmd_buffer[0] = RESP_START;
        cmd_buffer[2] = ' ';

        DD_PRINTF("Got a response for command type %d\r\n", cmdh_resp.cmd_type);

        switch(cmdh_resp.cmd_type) {
            case CMDH_TOGGLE_POWER:
                cmd_buffer[1] = CMD_POWER;
                cmd_buffer[3] = cmdh_resp.data.data ? '1' : '0';
                cmd_buffer[4] = RESP_END;
                cmd_buffer[5] = '\r';
                cmd_buffer[6] = '\n';
                cmd_buffer[7] = 0;

                USB_PRINTF(cmd_buffer);
                break;
            case CMDH_READ_PINS:
                cmd_buffer[1] = CMD_READ;
                strutils_u64_to_str(&cmd_buffer[3], cmdh_resp.data.data);
                cmd_buffer[19] = RESP_END;
                cmd_buffer[20] = '\r';
                cmd_buffer[21] = '\n';
                cmd_buffer[22] = 0;
                
                USB_PRINTF(cmd_buffer);
                break;
            case CMDH_WRITE_PINS:
                cmd_buffer[1] = CMD_WRITE;
                strutils_u64_to_str(&cmd_buffer[3], cmdh_resp.data.data);
                cmd_buffer[19] = RESP_END;
                cmd_buffer[20] = '\r';
                cmd_buffer[21] = '\n';
                cmd_buffer[22] = 0;
                
                USB_PRINTF(cmd_buffer);
                break;
            case CMDH_RESET: // Nothing to do
            default:
                break;
        }
    }
}

static void cli_parse_command(char cmd_buffer[CMD_BUFFER_SIZE], command_hub_queues *queues) {
    command_hub_cmd_resp cmdh_resp;

    switch(cmd_buffer[0]) {
        case CMD_MODEL:
            USB_PRINTF(RESP_MODEL);
            break;
        case CMD_TEST:
            // In this case we handle the response directly in this function
            cmd_buffer[0] = RESP_START;
            cmd_buffer[1] = CMD_TEST;
            cmd_buffer[2] = ' ';
            cmd_buffer[3] = cli_test_mode(queues) ? '1' : '0';
            cmd_buffer[4] = RESP_END;
            cmd_buffer[5] = '\r';
            cmd_buffer[6] = '\n';
            cmd_buffer[7] = 0;

            USB_PRINTF(cmd_buffer);
            break;
        case CMD_RESET:
            DD_PRINTF("Forcing an error state in the command hub...\r\n");
            xQueueSend(queues->cmd_queue, (void*)& ((command_hub_cmd){
                .type = CMDH_FORCE_ERROR,
                .data = 0,
                .id = 0
            }), portMAX_DELAY);
            break;
        case CMD_POWER: {
                bool relay_pwr = cmd_buffer[2] != '0'; // Check that we get something different than a '0'

                xQueueSend(queues->cmd_queue, (void*)& ((command_hub_cmd){
                    .type = CMDH_TOGGLE_POWER,
                    .data = relay_pwr,
                    .id = 0
                }), portMAX_DELAY);
            }
            break;
        case CMD_READ: {
                xQueueSend(queues->cmd_queue, (void*)& ((command_hub_cmd){
                    .type = CMDH_READ_PINS,
                    .data = 0,
                    .id = 0
                }), portMAX_DELAY);
            }
            break;
        case CMD_WRITE: {
                xQueueSend(queues->cmd_queue, (void*)& ((command_hub_cmd){
                    .type = CMDH_WRITE_PINS,
                    .data = strutils_str_to_u64(&cmd_buffer[2]),
                    .id = 0
                }), portMAX_DELAY);
            }   
            break;
        case CMD_EXT_WRITE: {
                for(uint8_t idx = 0; idx < 8; idx++) {
                    xQueueSend(queues->cmd_queue, (void*)& ((command_hub_cmd){
                        .type = CMDH_WRITE_PINS,
                        .data = strutils_str_to_u64(&cmd_buffer[2 + DATA_PARAMETER_SIZE * idx]),
                        .id = 0
                    }), portMAX_DELAY);                    
                }
            }
            break;
        default:
            USB_PRINTF(RESP_ERROR);
            break;
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
        .data = 1,
        .id = 0
    }), portMAX_DELAY);
    xQueueReceive(queues->resp_queue, (void*)&(cmdh_resp), portMAX_DELAY);

    for(uint8_t idx = 0; idx < tot_patterns; idx++) {
        DD_PRINTF("Testing %.16X.\r\n", test_patterns[idx]);
        xQueueSend(queues->cmd_queue, (void*)& ((command_hub_cmd){
            .type = CMDH_WRITE_PINS,
            .data = test_patterns[idx],
            .id = 0
        }), portMAX_DELAY);
        xQueueReceive(queues->resp_queue, (void*)&(cmdh_resp), portMAX_DELAY);

        if(cmdh_resp.data.data != test_patterns[idx]) {
            D_PRINTF("Failed pattern %llx, got %llx!\r\n", test_patterns[idx], cmdh_resp.data.data);

            test_result = false;
            break;
        }
    }

    // Disable the relay
    DD_PRINTF("Disabling the relay.\r\n");
    xQueueSend(queues->cmd_queue, (void*)& ((command_hub_cmd){
        .type = CMDH_TOGGLE_POWER,
        .data = 0,
        .id = 0
    }), portMAX_DELAY);
    xQueueReceive(queues->resp_queue, (void*)&(cmdh_resp), portMAX_DELAY);

    return test_result;
}