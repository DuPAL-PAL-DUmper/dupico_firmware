#include "binutils.h"

#include <string.h>
#include <pico.h>

uint64_t binutils_read_u64_le(uint8_t *buf) {
    uint64_t data = 0;
/*
    data |= (uint64_t)buf[0] << (8 * 0);
    data |= (uint64_t)buf[1] << (8 * 1);
    data |= (uint64_t)buf[2] << (8 * 2);
    data |= (uint64_t)buf[3] << (8 * 3);
    data |= (uint64_t)buf[4] << (8 * 4);
    data |= (uint64_t)buf[5] << (8 * 5);
    data |= (uint64_t)buf[6] << (8 * 6);
    data |= (uint64_t)buf[7] << (8 * 7);
*/
    memcpy(&data, buf, 8);
    return data;
}

void binutils_write_u64_le(uint8_t *buf, uint64_t data) {
    /*
    buf[0] = (data >> (8 * 0)) & 0xFF;
    buf[1] = (data >> (8 * 1)) & 0xFF;
    buf[2] = (data >> (8 * 2)) & 0xFF;
    buf[3] = (data >> (8 * 3)) & 0xFF;
    buf[4] = (data >> (8 * 4)) & 0xFF;
    buf[5] = (data >> (8 * 5)) & 0xFF;
    buf[6] = (data >> (8 * 6)) & 0xFF;
    buf[7] = (data >> (8 * 7)) & 0xFF;
    */
    memcpy(buf, &data, 8);
}
