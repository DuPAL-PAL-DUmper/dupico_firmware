#include "strutils.h"

#include <pico/platform.h>

const uint8_t __in_flash() ASCII_HEX_MAP[256 - 0x30] = { // Offset is 0x30
    0, 1, 2, 3, 4, 5, 6, 7, 8, 9,       // 0x30 - 0x39
    0, 0, 0, 0, 0, 0, 0, 10, 11, 12,    // 0x3A - 0x43
    13, 14, 15, 0, 0, 0, 0, 0, 0,       // 0x44 - 0x4C
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0,       // 0x4D - 0x56
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0,       // 0x57 - 0x60
    10, 11, 12, 13, 14, 15, 0, 0        // 0x61 - 0x66
};

const uint8_t __in_flash() HEX_ASCII_MAP[] = {
    0x30, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37, 0x38, 0x39,
    0x41, 0x42, 0x43, 0x44, 0x45, 0x46
};

uint8_t strutils_str_to_u8(char *str) {
    uint8_t res = 0;
    uint8_t diff = 0;
    
    res |= (((uint8_t)ASCII_HEX_MAP[str[0] - 0x30]) << (4-(4 * 0)));
    res |= (((uint8_t)ASCII_HEX_MAP[str[1] - 0x30]) << (4-(4 * 1)));

    return res;
}

uint32_t strutils_str_to_u32(char *str) {
    uint32_t res = 0;
    uint8_t diff = 0;

    res |= (((uint32_t)ASCII_HEX_MAP[str[0] - 0x30]) << (28-(4 * 0)));
    res |= (((uint32_t)ASCII_HEX_MAP[str[1] - 0x30]) << (28-(4 * 1)));
    res |= (((uint32_t)ASCII_HEX_MAP[str[2] - 0x30]) << (28-(4 * 2)));
    res |= (((uint32_t)ASCII_HEX_MAP[str[3] - 0x30]) << (28-(4 * 3)));
    res |= (((uint32_t)ASCII_HEX_MAP[str[4] - 0x30]) << (28-(4 * 4)));
    res |= (((uint32_t)ASCII_HEX_MAP[str[5] - 0x30]) << (28-(4 * 5)));
    res |= (((uint32_t)ASCII_HEX_MAP[str[6] - 0x30]) << (28-(4 * 6)));
    res |= (((uint32_t)ASCII_HEX_MAP[str[7] - 0x30]) << (28-(4 * 7)));

    return res;
}

uint64_t strutils_str_to_u64(char *str) {
    uint64_t res = 0;
    uint8_t diff = 0;

    res |= (((uint64_t)ASCII_HEX_MAP[str[0] - 0x30]) << (60-(4 * 0)));
    res |= (((uint64_t)ASCII_HEX_MAP[str[1] - 0x30]) << (60-(4 * 1)));
    res |= (((uint64_t)ASCII_HEX_MAP[str[2] - 0x30]) << (60-(4 * 2)));
    res |= (((uint64_t)ASCII_HEX_MAP[str[3] - 0x30]) << (60-(4 * 3)));
    res |= (((uint64_t)ASCII_HEX_MAP[str[4] - 0x30]) << (60-(4 * 4)));
    res |= (((uint64_t)ASCII_HEX_MAP[str[5] - 0x30]) << (60-(4 * 5)));
    res |= (((uint64_t)ASCII_HEX_MAP[str[6] - 0x30]) << (60-(4 * 6)));
    res |= (((uint64_t)ASCII_HEX_MAP[str[7] - 0x30]) << (60-(4 * 7)));
    res |= (((uint64_t)ASCII_HEX_MAP[str[8] - 0x30]) << (60-(4 * 8)));
    res |= (((uint64_t)ASCII_HEX_MAP[str[9] - 0x30]) << (60-(4 * 9)));
    res |= (((uint64_t)ASCII_HEX_MAP[str[10] - 0x30]) << (60-(4 * 10)));
    res |= (((uint64_t)ASCII_HEX_MAP[str[11] - 0x30]) << (60-(4 * 11)));
    res |= (((uint64_t)ASCII_HEX_MAP[str[12] - 0x30]) << (60-(4 * 12)));
    res |= (((uint64_t)ASCII_HEX_MAP[str[13] - 0x30]) << (60-(4 * 13)));
    res |= (((uint64_t)ASCII_HEX_MAP[str[14] - 0x30]) << (60-(4 * 14)));
    res |= (((uint64_t)ASCII_HEX_MAP[str[15] - 0x30]) << (60-(4 * 15)));

    return res;
}

void strutils_u8_to_str(char *str, uint8_t data) {
    str[0] = HEX_ASCII_MAP[(data >> ((1 - 0)*4)) & 0x0F];
    str[1] = HEX_ASCII_MAP[(data >> ((1 - 1)*4)) & 0x0F];
}

void strutils_u32_to_str(char *str, uint32_t data) {
    str[0] = HEX_ASCII_MAP[(data >> ((7 - 0)*4)) & 0x0F];
    str[1] = HEX_ASCII_MAP[(data >> ((7 - 1)*4)) & 0x0F];
    str[2] = HEX_ASCII_MAP[(data >> ((7 - 2)*4)) & 0x0F];
    str[3] = HEX_ASCII_MAP[(data >> ((7 - 3)*4)) & 0x0F];
    str[4] = HEX_ASCII_MAP[(data >> ((7 - 4)*4)) & 0x0F];
    str[5] = HEX_ASCII_MAP[(data >> ((7 - 5)*4)) & 0x0F];
    str[6] = HEX_ASCII_MAP[(data >> ((7 - 6)*4)) & 0x0F];
    str[7] = HEX_ASCII_MAP[(data >> ((7 - 7)*4)) & 0x0F];
}

void strutils_u64_to_str(char *str, uint64_t data) {
    str[0] = HEX_ASCII_MAP[(data >> ((15 - 0)*4)) & 0x0F];
    str[1] = HEX_ASCII_MAP[(data >> ((15 - 1)*4)) & 0x0F];
    str[2] = HEX_ASCII_MAP[(data >> ((15 - 2)*4)) & 0x0F];
    str[3] = HEX_ASCII_MAP[(data >> ((15 - 3)*4)) & 0x0F];
    str[4] = HEX_ASCII_MAP[(data >> ((15 - 4)*4)) & 0x0F];
    str[5] = HEX_ASCII_MAP[(data >> ((15 - 5)*4)) & 0x0F];
    str[6] = HEX_ASCII_MAP[(data >> ((15 - 6)*4)) & 0x0F];
    str[7] = HEX_ASCII_MAP[(data >> ((15 - 7)*4)) & 0x0F];
    str[8] = HEX_ASCII_MAP[(data >> ((15 - 8)*4)) & 0x0F];
    str[9] = HEX_ASCII_MAP[(data >> ((15 - 9)*4)) & 0x0F];
    str[10] = HEX_ASCII_MAP[(data >> ((15 - 10)*4)) & 0x0F];
    str[11] = HEX_ASCII_MAP[(data >> ((15 - 11)*4)) & 0x0F];
    str[12] = HEX_ASCII_MAP[(data >> ((15 - 12)*4)) & 0x0F];
    str[13] = HEX_ASCII_MAP[(data >> ((15 - 13)*4)) & 0x0F];
    str[14] = HEX_ASCII_MAP[(data >> ((15 - 14)*4)) & 0x0F];
    str[15] = HEX_ASCII_MAP[(data >> ((15 - 15)*4)) & 0x0F];
}