#include "data_structs.h"

#include <pico/platform.h>

#define TYPE_NAME_LEN 10
static const char __in_flash() ic_type_name_array[][TYPE_NAME_LEN] = {
    "ROM",
    "SRAM"
};

static inline uint64_t map_to_mask_conversion(uint32_t src, uint8_t max_len, uint8_t *map);
static inline uint32_t mask_to_map_conversion(uint64_t mask, uint8_t max_len, uint8_t *map);

static inline uint64_t map_to_mask_conversion(uint32_t src, uint8_t max_len, uint8_t *map) {
    uint64_t mask = 0;

    for(uint8_t idx = 0; idx < max_len; idx++) {
        mask |= (src & (1 << idx)) ? (1 << map[idx]) : 0;
    }

    return mask;
}

static inline uint32_t mask_to_map_conversion(uint64_t mask, uint8_t max_len, uint8_t *map) {
    uint32_t data = 0;

    for(uint8_t idx = 0; idx < max_len; idx++) {
        data |= (mask & (1 << map[idx])) ? (1 << idx) : 0;
    }

    return data;
}

uint64_t ctrl_struct_i_to_mask(IC_Ctrl_Struct *iccd, uint32_t i) {
    return map_to_mask_conversion(i, MIN(iccd->i_len, MAX_I_LEN), iccd->i_map);
}

uint32_t ctrl_struct_mask_to_i(IC_Ctrl_Struct *iccd, uint64_t mask) {
    return mask_to_map_conversion(mask, MIN(iccd->i_len, MAX_I_LEN), iccd->i_map);
}

uint64_t ctrl_struct_io_to_mask(IC_Ctrl_Struct *iccd, uint16_t io) {
    return map_to_mask_conversion(io, MIN(iccd->io_len, MAX_IO_LEN), iccd->io_map);
}

uint16_t ctrl_struct_mask_to_io(IC_Ctrl_Struct *iccd, uint64_t mask) {
    return mask_to_map_conversion(mask, MIN(iccd->io_len, MAX_IO_LEN), iccd->io_map) & 0xFFFF;
}

uint64_t ctrl_struct_pwr_to_mask(IC_Ctrl_Struct *iccd, uint8_t pwr) {
    return map_to_mask_conversion(pwr, MIN(iccd->pwr_len, MAX_PWR_LEN), iccd->pwr_map);
}

uint8_t ctrl_struct_mask_to_pwr(IC_Ctrl_Struct *iccd, uint64_t mask) {
    return mask_to_map_conversion(mask, MIN(iccd->pwr_len, MAX_PWR_LEN), iccd->pwr_map) & 0xFF;
}

uint64_t ctrl_struct_ctrl_to_mask(IC_Ctrl_Struct *iccd, uint8_t ctrl) {
    return map_to_mask_conversion(ctrl, MIN(iccd->ctrl_len, MAX_PWR_LEN), iccd->ctrl_map);
}

uint8_t ctrl_struct_mask_to_ctrl(IC_Ctrl_Struct *iccd, uint64_t mask) {
    return mask_to_map_conversion(mask, MIN(iccd->ctrl_len, MAX_CTRL_LEN), iccd->ctrl_map) & 0xFF;
}

const char *get_ic_type_name(uint16_t type) {
    if(type >= sizeof(ic_type_name_array)/TYPE_NAME_LEN) {
        return "GENERIC";
    } else {
        return ic_type_name_array[type];
    }
}

