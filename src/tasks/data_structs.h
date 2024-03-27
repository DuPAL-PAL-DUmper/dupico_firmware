#ifndef _DATA_STRUCTS_HEADER_
#define _DATA_STRUCTS_HEADER_

#include <stdint.h>

typedef struct {
    char name[16];
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
} IC_Control_Data __attribute__((packed));

#endif /* _DATA_STRUCTS_HEADER_ */