#include "cxfer.h"

#include <string.h>

#include <pico.h>
#include <pico/stdlib.h>
#include <pico/stdio/driver.h>
#include <pico/stdio.h>
#include <pico/stdio_usb.h>

#include <utils/custom_debug.h>

#define ADDRESS_SHIFT_BUFFER_SIZE 64
#define DATA_SHIFT_BUFFER_SIZE 64
#define SHIFT_BLOCK_SIZE 16
#define XMIT_BLOCK_SIZE 1024

const uint8_t __in_flash() XFER_PKT_START[] = {0xDE, 0xAD, 0xBE, 0xEF};
const uint8_t __in_flash() XFER_PKT_FAIL[]  = {0xBA, 0xAD, 0xF0, 0x0D};
const uint8_t __in_flash() XFER_DONE[]      = {0xC0, 0x0F, 0xFF, 0xEE};

typedef struct {
    uint8_t addr_shift_buffer[ADDRESS_SHIFT_BUFFER_SIZE];
    uint8_t data_shift_buffer[DATA_SHIFT_BUFFER_SIZE];
    uint64_t hi_out_mask;
    uint64_t data_mask;
    uint8_t addr_width;
    uint8_t data_width;
} cxfer_cfg;

static cxfer_cfg cfg;

static uint64_t map_value(uint8_t shift_map_len, const uint8_t shift_map[], uint64_t mask, uint64_t val, bool reverse);
static uint8_t xfer_read(cxfer_cfg *cfg, command_hub_queues* queues);

uint8_t cxfer_execute_subcommand(cxfer_subcmd scmd, uint8_t *params, command_hub_queues* queues) {
    DD_PRINTF("Got subcommand %.2X\r\n", scmd);

    switch(scmd) {
        case SET_ADDR_MAP_0:
            memcpy(&(cfg.addr_shift_buffer[SHIFT_BLOCK_SIZE * 0]), params, SHIFT_BLOCK_SIZE);
            break;
        case SET_ADDR_MAP_1:
            memcpy(&(cfg.addr_shift_buffer[SHIFT_BLOCK_SIZE * 1]), params, SHIFT_BLOCK_SIZE);
            break;
        case SET_ADDR_MAP_2:
            memcpy(&(cfg.addr_shift_buffer[SHIFT_BLOCK_SIZE * 2]), params, SHIFT_BLOCK_SIZE);
            break;
        case SET_ADDR_MAP_3:
            memcpy(&(cfg.addr_shift_buffer[SHIFT_BLOCK_SIZE * 3]), params, SHIFT_BLOCK_SIZE);
            break;
        case SET_DATA_MAP_0:
            memcpy(&(cfg.data_shift_buffer[SHIFT_BLOCK_SIZE * 0]), params, SHIFT_BLOCK_SIZE);
            break;
        case SET_DATA_MAP_1:
            memcpy(&(cfg.data_shift_buffer[SHIFT_BLOCK_SIZE * 1]), params, SHIFT_BLOCK_SIZE);
            break;
        case SET_DATA_MAP_2:
            memcpy(&(cfg.data_shift_buffer[SHIFT_BLOCK_SIZE * 2]), params, SHIFT_BLOCK_SIZE);
            break;
        case SET_DATA_MAP_3:
            memcpy(&(cfg.data_shift_buffer[SHIFT_BLOCK_SIZE * 3]), params, SHIFT_BLOCK_SIZE);
            break;
        case SET_HI_OUT_MASK:
            memcpy(&(cfg.hi_out_mask), params, sizeof(uint64_t));        
            break;
        case SET_DATA_MASK:
            memcpy(&(cfg.data_mask), params, sizeof(uint64_t));        
            break;
        case SET_ADDR_WIDTH:
            cfg.addr_width = params[0];
            break;
        case SET_DATA_WIDTH:
            cfg.data_width = params[0];
            break;
        case CLEAR:
            memset(&cfg, 0, sizeof(cxfer_cfg));
            break;
        case EXECUTE_WRITE:
            // TODO
            D_PRINTF("Request for EXECUTE_WRITE is not supported!\r\n");
            break;
        case EXECUTE_READ:
            return xfer_read(&cfg, queues);
        default:
            D_PRINTF("Unrecognized subcommand %.2X\r\n", scmd);
            break;
    }

    return 0;
}

static uint64_t map_value(uint8_t shift_map_len, const uint8_t shift_map[], uint64_t mask, uint64_t val, bool reverse) {
    uint64_t remapped_value = 0;

    val &= mask;

    if(reverse) {
        for(uint8_t idx = 0; idx < shift_map_len; idx++) {
            remapped_value |= (val & (1ULL << shift_map[idx])) ? (1ULL << idx) : 0;
        }
    } else {
        for(uint8_t idx = 0; idx < shift_map_len; idx++) {
            remapped_value |= (val & (1ULL << idx)) ? (1ULL << shift_map[idx]) : 0;
        }
    }

    return remapped_value;
}

static uint8_t xfer_read(cxfer_cfg *cfg, command_hub_queues* queues) {
    command_hub_cmd_resp cmdh_resp;
    
    uint8_t data_byte_width = (cfg->data_width / 8) + ((cfg->data_width % 8) ? 1 : 0);
    uint32_t addr_combs = 1 << cfg->addr_width;
    uint32_t file_size = data_byte_width * addr_combs;
    uint32_t tot_blocks = (file_size / XMIT_BLOCK_SIZE) + ((file_size % XMIT_BLOCK_SIZE) ? 1 : 0);
    uint16_t checksum;
    uint8_t rcvd_checksum_buf[2];

    uint64_t data;
    uint32_t cur_address = 0;

    D_PRINTF("Clearing the input buffer...\r\n");
    while(getchar_timeout_us(0) >= 0); // Clear the input buffer

    uint32_t block_byte;
    for(uint32_t cur_block = 0; cur_block < tot_blocks; cur_block++) {
        stdio_usb.out_chars(XFER_PKT_START, sizeof(XFER_PKT_START));
        checksum = 0;

        for(block_byte = 0; (block_byte < XMIT_BLOCK_SIZE) && (cur_address < addr_combs); block_byte += data_byte_width, cur_address++) {
            data = map_value(cfg->addr_width, cfg->addr_shift_buffer, 0xFFFFFFFFFFFFFFFFULL, cur_address, false) | cfg->hi_out_mask;
            xQueueSend(queues->cmd_queue, (void*)& ((command_hub_cmd){
                .type = CMDH_WRITE_PINS,
                .data = data
            }), portMAX_DELAY);

            if(!xQueueReceive(queues->resp_queue, (void*)&(cmdh_resp), portMAX_DELAY)) {
                D_PRINTF("No response from the command hub!!\r\n");
                stdio_usb.out_chars(XFER_PKT_FAIL, sizeof(XFER_PKT_FAIL));
                return 0;
            }

            // Reverse map the data we've read
            data = map_value(cfg->data_width, cfg->data_shift_buffer, 0xFFFFFFFFFFFFFFFFULL, cmdh_resp.data.data, true);
            // Send data through serial
            for (uint8_t data_idx = 0; data_idx < data_byte_width; data_idx++) {
                uint8_t data_b = (data >> (8 * data_idx)) & 0xFF;
                checksum += data_b; // Update the checksum
                putchar_raw(data_b);
            }
        }

        // Take care of filling remaining bytes of the last block with 0s
        for(; block_byte < XMIT_BLOCK_SIZE; block_byte++) {
            putchar_raw(0);
        }

        stdio_usb.out_chars((uint8_t*)&checksum, sizeof(checksum));
        taskYIELD();

        // Read the checksum as an ACK
        int chk_count = sizeof(rcvd_checksum_buf), retries = 5;
        while(chk_count > 0 && retries--) {
            int read_count = stdio_usb.in_chars(rcvd_checksum_buf, chk_count);
            if(read_count > 0) {
                chk_count -= read_count;
            }
            vTaskDelay(10);
        }

        if (!retries) {
            D_PRINTF("Timed out while waiting for checksum response!\r\n");
            stdio_usb.out_chars(XFER_PKT_FAIL, sizeof(XFER_PKT_FAIL));
            return 0;
        }

        uint16_t rcvd_checksum = rcvd_checksum_buf[0] | ((uint16_t)rcvd_checksum_buf[1]) << 8;
    
        if(rcvd_checksum != checksum) {
            D_PRINTF("Sent checksum %.4X, but read back %.4X!!!\r\n", checksum, rcvd_checksum);
            stdio_usb.out_chars(XFER_PKT_FAIL, sizeof(XFER_PKT_FAIL));
            return 0;
        } else {
            DD_PRINTF("Checksum received successfully!!\r\n");
        }
    }
        
    stdio_usb.out_chars(XFER_DONE, sizeof(XFER_DONE));
    return 1;
}