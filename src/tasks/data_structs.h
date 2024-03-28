#ifndef _DATA_STRUCTS_HEADER_
#define _DATA_STRUCTS_HEADER_

#include <stdint.h>
#include <pico/types.h>

typedef struct __attribute__((packed)) {
    char name[16];
    uint16_t chip_type;
    uint8_t address_len;
    uint8_t address_idx;
    uint8_t data_len;
    uint8_t data_idx;
    uint8_t power_len;
    uint8_t power_idx;
    uint8_t special_len;
    uint8_t special_map_idx;
    uint8_t special_type_idx;
    uint8_t data[];
    // Where data is 
    // address_len + data_len + power_len + (special_len * 2) long
} IC_Control_Data;

uint calculate_IC_Control_Data_size(IC_Control_Data *iccd);

#endif /* _DATA_STRUCTS_HEADER_ */