#include "cli_interface_task.h"

#include <string.h>

#include <FreeRTOS.h>
#include <queue.h>
#include <task.h>

#include "pico/stdio_usb.h"

#include "tasks/command_hub_task.h"
#include "utils/custom_debug.h"

#define VERSION "0.0.1"
#define SOFT_HEADER "\nDuPICO - " VERSION "\n\n\r"

#define MODEL "3"

#define CMD_BUFFER_SIZE 64
#define PKT_START '>'
#define PKT_END '<'
#define RESP_START '['
#define RESP_END ']'

#define CMD_WRITE 'W'
#define CMD_READ 'R'
#define CMD_EXIT 'X'
#define CMD_RESET 'K'
#define CMD_MODEL 'M'
#define CMD_LED 'L'

#define RESP_ERROR "CMD_ERR\n\r"
#define RESP_MODEL "[M " MODEL "]\n\r"

static char cmd_buffer[CMD_BUFFER_SIZE];

static void cli_parse_command(char cmd_buffer[CMD_BUFFER_SIZE], command_hub_queues *queues);

void cli_interface_task(void *params) {
    command_hub_queues *queues = (command_hub_queues*)params; 
    command_hub_cmd_resp cmdh_resp;

    uint8_t buf_idx = 0;
    bool receiving_cmd = false;
    uint32_t current_cmd_id = 0;
    bool term_connected_state = false;
    bool cur_term_connected = false;
    int ch;

    while(true) {
        cur_term_connected = stdio_usb_connected();

        if(cur_term_connected != term_connected_state) {
            term_connected_state = cur_term_connected;

            if(!term_connected_state) { // We got a disconnection from the device
                D_PRINTF("Serial terminal disconnected!\r\n");
            } else { // New connection!
                D_PRINTF("Serial terminal connected!\r\n");

                buf_idx = 0;
                receiving_cmd = false;

                // Tell the command hub to reset
                xQueueSend(queues->cmd_queue, (void*)& ((command_hub_cmd){
                    .type = CMDH_RESET,
                    .data = 0,
                    .id = current_cmd_id++
                }), portMAX_DELAY);

                if(!xQueueReceive(queues->resp_queue, (void*)&(cmdh_resp), portMAX_DELAY)) {
                    D_PRINTF("Error requesting a reset from the command hub\r\n");
                } else {
                    D_PRINTF("Reset from command hub responded with %u - %u\r\n", cmdh_resp.id, cmdh_resp.type);
                }

                USB_PRINTF(SOFT_HEADER);
                USB_PRINTF("REMOTE_CONTROL_ENABLED\r\n");
            }
        }

        if(term_connected_state && ((ch = getchar_timeout_us(0)) >= 0)) {
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

        taskYIELD();
    } 
}

static void cli_parse_command(char cmd_buffer[CMD_BUFFER_SIZE], command_hub_queues *queues) {
    switch(cmd_buffer[0]) {
        case CMD_MODEL:
            USB_PRINTF(RESP_MODEL);
            break;
        case CMD_READ:
        case CMD_WRITE:
        case CMD_LED:
        case CMD_RESET:
        case CMD_EXIT:
        default:
            USB_PRINTF(RESP_ERROR);
            break;
    }
}