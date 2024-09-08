#ifndef _CXFER_HEADER_
#define _CXFER_HEADER_

#include <stdint.h>

#include <FreeRTOS.h>
#include <queue.h>

#include <tasks/command_hub_task.h>

typedef enum {
    SET_ADDR_MAP_0 = 0x00, // shift indices for bits 0-15, address
    SET_ADDR_MAP_1 = 0x01, // shift indices for bits 16-31
    SET_ADDR_MAP_2 = 0x02, // shift indices for bits 32-47
    SET_ADDR_MAP_3 = 0x03, // shift indices for bits 48-63 
    SET_DATA_MAP_0 = 0x10, // shift indices for bits 0-15, data
    SET_DATA_MAP_1 = 0x11, // shift indices for bits 16-31
    SET_DATA_MAP_2 = 0x12, // shift indices for bits 32-47
    SET_DATA_MAP_3 = 0x13, // shift indices for bits 48-63   
    SET_HI_OUT_MASK = 0xE0,
    SET_DATA_MASK = 0xE1,
    SET_ADDR_WIDTH = 0xE2,
    SET_DATA_WIDTH = 0xE3,
    CLEAR = 0xF0,
    EXECUTE_WRITE = 0xFE,
    EXECUTE_READ = 0xFF
} cxfer_subcmd;

uint8_t cxfer_execute_subcommand(cxfer_subcmd scmd, uint8_t *params, command_hub_queues* queues);

#endif /* _CXFER_HEADER_ */