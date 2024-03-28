#ifndef _DATA_STRUCTS_HEADER_
#define _DATA_STRUCTS_HEADER_

#include <stdint.h>
#include <pico/types.h>

typedef struct __attribute__((packed)) {
    char name[16];
    uint16_t chip_type;
    uint8_t i_len;
    uint8_t i_idx;
    uint8_t io_len;
    uint8_t io_idx;
    uint8_t pwr_len;
    uint8_t pwr_idx;
    uint8_t ctrl_len;
    uint8_t ctrl_map_idx;
    uint8_t ctrl_type_idx;
    uint8_t data[];
    // Where data is 
    // i_len + io_len + pwr_len + (ctrl_len * 2) long
} IC_Ctrl_Struct;

uint calculate_IC_Ctrl_Struct_size(IC_Ctrl_Struct *iccd);

uint64_t ctrl_struct_i_to_mask(IC_Ctrl_Struct *iccd, uint64_t i);
uint64_t ctrl_struct_io_to_mask(IC_Ctrl_Struct *iccd, uint64_t io);
uint64_t ctrl_struct_mask_to_io(IC_Ctrl_Struct *iccd, uint64_t mask);
uint64_t ctrl_struct_pwr_to_mask(IC_Ctrl_Struct *iccd, uint64_t pwr);
uint64_t ctrl_struct_ctrl_to_mask(IC_Ctrl_Struct *iccd, uint64_t ctrl);

#endif /* _DATA_STRUCTS_HEADER_ */