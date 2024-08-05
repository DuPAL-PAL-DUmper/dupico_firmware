#include "bin_cmd_handler.h"

#include <FreeRTOS.h>

#include <string.h>

#include <pico/platform.h>
#include <pico/stdio.h>
#include <pico/stdio_usb.h>

#include <utils/custom_debug.h>
#include <utils/binutils.h>

#define BIN_CMD_RESPONSE_MASK 0x80

#define BIN_CMD_WRITE 0x00
#define BIN_CMD_READ 0x01
#define BIN_CMD_RESET 0x02
#define BIN_CMD_POWER 0x03
#define BIN_CMD_MODEL 0x04
#define BIN_CMD_TEST 0x05
#define BIN_CMD_VERSION 0x06
#define BIN_CMD_SWITCH_PROTO 0x07

#define BIN_CMD_ERROR 0xFF

#define CMD_WRITE_LEN 10
#define CMD_READ_LEN 2
#define CMD_RESET_LEN 2
#define CMD_POWER_LEN 3
#define CMD_MODEL_LEN 2
#define CMD_TEST_LEN 2
#define CMD_VERSION_LEN 2
#define CMD_SWITCH_PROTO_LEN 3

#define RESP_WRITE_LEN 9
#define RESP_READ_LEN 9
#define RESP_RESET_LEN 1
#define RESP_POWER_LEN 2
#define RESP_MODEL_LEN 2
#define RESP_TEST_LEN 2
#define RESP_VERSION_LEN 11
#define RESP_SWITCH_PROTO_LEN 2

// This map reports the expected length for every command
const uint8_t __in_flash() CMD_LEN_MAP[] = {
    CMD_WRITE_LEN,
    CMD_READ_LEN,
    CMD_RESET_LEN,
    CMD_POWER_LEN,
    CMD_MODEL_LEN,
    CMD_TEST_LEN,
    CMD_VERSION_LEN,
    CMD_SWITCH_PROTO_LEN
};

const uint8_t __in_flash() RESP_LEN_MAP[] = {
    RESP_WRITE_LEN,
    RESP_READ_LEN,
    RESP_RESET_LEN,
    RESP_POWER_LEN,
    RESP_MODEL_LEN,
    RESP_TEST_LEN,
    RESP_VERSION_LEN,
    RESP_SWITCH_PROTO_LEN
};

static inline void reset_handler_config(handler_config* config);
static uint16_t bin_parse_command(handler_config* config);
static void transmit_response(handler_config* config);
static uint8_t checksum(const uint8_t *buf, uint16_t len);

uint16_t bin_cmd_handler(handler_config* config) {
    // If we have a spurious byte of an unrecognized command, just return an error and reset the state
    if(config->cmd_buffer[0] > sizeof(CMD_LEN_MAP)) {
        reset_handler_config(config);
        putchar_raw(BIN_CMD_ERROR); // Return an error, the command is unrecognized
        return CMD_HANDLER_ERROR;
    } else if (config->buf_idx >= CMD_LEN_MAP[config->cmd_buffer[0]]) { // We have enough data for this command
        if(checksum(config->cmd_buffer, CMD_LEN_MAP[config->cmd_buffer[0]])) { // The command did not pass the checksum
            reset_handler_config(config);
            return CMD_HANDLER_ERROR;
        }

        uint16_t res = bin_parse_command(config);
        reset_handler_config(config);

        return res;
    }

    return CMD_HANDLER_NO_CMD;   
}

void bin_response_handler(handler_config* config) {
    bool response_ready = false;
    command_hub_cmd_resp cmdh_resp;

    while(xQueueReceive(config->queues->resp_queue, (void*)&(cmdh_resp), 0)) {
        DD_PRINTF("Got a response for command type %d\r\n", cmdh_resp.cmd_type);
        switch(cmdh_resp.cmd_type) {
            case CMDH_TOGGLE_POWER:
                config->cmd_buffer[0] = BIN_CMD_POWER;
                config->cmd_buffer[1] = cmdh_resp.data.data ? 1 : 0;
                response_ready = true;
                break;
            case CMDH_READ_PINS:
                config->cmd_buffer[0] = BIN_CMD_READ;
                binutils_write_u64_le(&(config->cmd_buffer[1]), cmdh_resp.data.data);
                response_ready = true;
                break;
            case CMDH_WRITE_PINS:
                config->cmd_buffer[0] = BIN_CMD_WRITE;
                binutils_write_u64_le(&(config->cmd_buffer[1]), cmdh_resp.data.data);
                response_ready = true;
                break;
            case CMDH_RESET: // Nothing to do
            default:
                reset_handler_config(config);
                break;
        }
    }    
    
    if(response_ready) {
        transmit_response(config);
        reset_handler_config(config);
        
    }
}

static inline void reset_handler_config(handler_config* config) {
    config->buf_idx = 0;
}

static uint16_t bin_parse_command(handler_config* config) {
    uint16_t resp = CMD_HANDLER_RESP;
    bool response_ready = false; // If true, we don't need to wait for any data in bin_response_handler to send a response

    switch(config->cmd_buffer[0]) {
        case BIN_CMD_WRITE:
            xQueueSend(config->queues->cmd_queue, (void*)& ((command_hub_cmd){
                .type = CMDH_WRITE_PINS,
                .data = binutils_read_u64_le(&(config->cmd_buffer[1]))
            }), portMAX_DELAY);
            break;
        case BIN_CMD_READ:
            xQueueSend(config->queues->cmd_queue, (void*)& ((command_hub_cmd){
                .type = CMDH_READ_PINS,
                .data = 0
            }), portMAX_DELAY);
            break;
        case BIN_CMD_RESET:
            DD_PRINTF("Forcing an error state in the command hub...\r\n");
            xQueueSend(config->queues->cmd_queue, (void*)& ((command_hub_cmd){
                .type = CMDH_FORCE_ERROR,
                .data = 0
            }), portMAX_DELAY);
            response_ready = true;
            break;
        case BIN_CMD_POWER: {
                bool relay_pwr = config->cmd_buffer[1] != 0; // Check that we get something different than a 0

                xQueueSend(config->queues->cmd_queue, (void*)& ((command_hub_cmd){
                    .type = CMDH_TOGGLE_POWER,
                    .data = relay_pwr
                }), portMAX_DELAY);
            }
            break;
        case BIN_CMD_VERSION:
            strncpy(&(config->cmd_buffer[1]), FW_VERSION, RESP_VERSION_LEN - 1);
            response_ready = true;
            break;
        case BIN_CMD_MODEL:
            config->cmd_buffer[1] = HW_MODEL & 0xFF;
            response_ready = true;
            break;
        case BIN_CMD_TEST:
            config->cmd_buffer[1] = config->test_func(config->queues) ? 1 : 0;
            response_ready = true;
            break;
        case BIN_CMD_SWITCH_PROTO:
            resp = CMD_HANDLER_SWITCH_PROTO | config->cmd_buffer[1];
            response_ready = true;
            break;
        default: 
            reset_handler_config(config);
            break;
    }

    if(response_ready) {
        transmit_response(config);
        reset_handler_config(config);        
    }

    return resp;
}

static void transmit_response(handler_config* config) {
    uint8_t resp_len = RESP_LEN_MAP[config->cmd_buffer[0]];
    // Mark the byte as a response
    config->cmd_buffer[0] |= BIN_CMD_RESPONSE_MASK;
    config->cmd_buffer[resp_len] = checksum(config->cmd_buffer, resp_len);
    stdio_usb.out_chars(config->cmd_buffer, resp_len + 1);
}

static uint8_t checksum(const uint8_t *buf, uint16_t len) {
    uint8_t checksum = 0;
    while (len-- != 0)
        checksum -= *buf++;
    return checksum;    
}