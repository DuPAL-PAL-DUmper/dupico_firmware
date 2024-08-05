#ifndef _CMD_HANDLER_HEADER_
#define _CMD_HANDLER_HEADER_

#include <stdint.h>

#include "pico/stdlib.h"

#include <tasks/command_hub_task.h>

typedef bool (*test_cmd_func)(command_hub_queues* queues);

typedef struct {
    const uint16_t buf_size;
    uint8_t *cmd_buffer;
    uint16_t buf_idx;

    command_hub_queues *queues;

    // Some functions are executed directly by the handler, we pass them as pointers here
    test_cmd_func test_func;
} handler_config;

#define CMD_HANDLER_ERROR 0x0000 // Something went wrong
#define CMD_HANDLER_NO_CMD 0x0001 // Got data, but no command was executed yet
#define CMD_HANDLER_RESP 0x0100 // We executed a command and have a response to relay. The LSB contains the response. If just "OK" it'll be 00
#define CMD_HANDLER_SWITCH_PROTO 0xFF00 // Request to switch command protocol received. The LSB contains the code for the new proto to use

typedef uint16_t (*cmd_handler)(handler_config* config);
typedef void (*cmd_response_handler)(handler_config* config);

typedef struct {
    cmd_handler c_handler;
    cmd_response_handler cr_handler;
} handler_funcs;

#endif /* _CMD_HANDLER_HEADER_ */