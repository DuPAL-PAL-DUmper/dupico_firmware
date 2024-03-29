#ifndef _DATA_STRUCTS_HEADER_
#define _DATA_STRUCTS_HEADER_

#include <stdint.h>
#include <pico/types.h>

#define IC_NAME_LEN 16
#define MAX_I_LEN 24
#define MAX_IO_LEN 16
#define MAX_PWR_LEN 8
#define MAX_CTRL_LEN 8

typedef struct __attribute__((packed)) {
    char name[IC_NAME_LEN];
    uint16_t chip_type;
    uint8_t i_len;
    uint8_t i_map[MAX_I_LEN];
    uint8_t io_len;
    uint8_t io_map[MAX_IO_LEN];
    uint8_t pwr_len;
    uint8_t pwr_map[MAX_PWR_LEN];
    uint8_t ctrl_len;
    uint8_t ctrl_map[MAX_CTRL_LEN];
    uint8_t ctrl_type[MAX_CTRL_LEN];
} IC_Ctrl_Struct;

uint64_t ctrl_struct_i_to_mask(IC_Ctrl_Struct *iccd, uint64_t i);
uint64_t ctrl_struct_io_to_mask(IC_Ctrl_Struct *iccd, uint64_t io);
uint64_t ctrl_struct_mask_to_io(IC_Ctrl_Struct *iccd, uint64_t mask);
uint64_t ctrl_struct_pwr_to_mask(IC_Ctrl_Struct *iccd, uint64_t pwr);
uint64_t ctrl_struct_ctrl_to_mask(IC_Ctrl_Struct *iccd, uint64_t ctrl);

#endif /* _DATA_STRUCTS_HEADER_ */