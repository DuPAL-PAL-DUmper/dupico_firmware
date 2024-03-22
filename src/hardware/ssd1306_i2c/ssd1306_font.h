/**
 * Copyright (c) 2022 Raspberry Pi (Trading) Ltd.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

// Vertical bitmaps, A-Z, 0-9. Each is 8 pixels high and wide
// Theses are defined vertically to make them quick to copy to FB

#include <stdint.h>

static uint8_t font[] = {
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,  // Nothing
0x1e, 0x28, 0x48, 0x88, 0x48, 0x28, 0x1e, 0x00,  //A
0xfe, 0x92, 0x92, 0x92, 0x92, 0x92, 0xfe, 0x00,  //B
0x7e, 0x82, 0x82, 0x82, 0x82, 0x82, 0x82, 0x00,  //C
0xfe, 0x82, 0x82, 0x82, 0x82, 0x82, 0x7e, 0x00,  //D
0xfe, 0x92, 0x92, 0x92, 0x92, 0x92, 0x92, 0x00,  //E
0xfe, 0x90, 0x90, 0x90, 0x90, 0x80, 0x80, 0x00,  //F
0xfe, 0x82, 0x82, 0x82, 0x8a, 0x8a, 0xce, 0x00,  //G
0xfe, 0x10, 0x10, 0x10, 0x10, 0x10, 0xfe, 0x00,  //H
0x00, 0x00, 0x00, 0xfe, 0x00, 0x00, 0x00, 0x00,  //I
0x84, 0x82, 0x82, 0xfc, 0x80, 0x80, 0x80, 0x00,  //J
0x00, 0xfe, 0x10, 0x10, 0x28, 0x44, 0x82, 0x00,  //K
0xfe, 0x02, 0x02, 0x02, 0x02, 0x02, 0x02, 0x00,  //L
0xfe, 0x40, 0x20, 0x10, 0x20, 0x40, 0xfe, 0x00,  //M
0xfe, 0x40, 0x20, 0x10, 0x08, 0x04, 0xfe, 0x00,  //N
0x7c, 0x82, 0x82, 0x82, 0x82, 0x82, 0x7c, 0x00,  //O
0xfe, 0x88, 0x88, 0x88, 0x88, 0x88, 0x70, 0x00,  //P
0x7c, 0x82, 0x82, 0x92, 0x8a, 0x86, 0x7e, 0x00,  //Q
0xfe, 0x88, 0x88, 0x88, 0x8c, 0x8a, 0x70, 0x00,  //R
0x62, 0x92, 0x92, 0x92, 0x92, 0x0c, 0x00, 0x00,  //S
0x80, 0x80, 0x80, 0xfe, 0x80, 0x80, 0x80, 0x00,  //T
0xfc, 0x02, 0x02, 0x02, 0x02, 0x02, 0xfc, 0x00,  //U
0xf0, 0x08, 0x04, 0x02, 0x04, 0x08, 0xf0, 0x00,  //V
0xfe, 0x04, 0x08, 0x10, 0x08, 0x04, 0xfe, 0x00,  //W
0x00, 0x82, 0x44, 0x28, 0x28, 0x44, 0x82, 0x00,  //X
0x80, 0x40, 0x20, 0x1e, 0x20, 0x40, 0x80, 0x00,  //Y
0x82, 0x86, 0x9a, 0xa2, 0xc2, 0x82, 0x00, 0x00,  //Z
0x7c, 0x82, 0x82, 0x92, 0x82, 0x82, 0x7c, 0x00,  //0
0x00, 0x00, 0x42, 0xfe, 0x02, 0x00, 0x00, 0x00,  //1
0x0c, 0x92, 0x92, 0x92, 0x92, 0x62, 0x00, 0x00,  //2
0x92, 0x92, 0x92, 0x92, 0x92, 0x92, 0x6c, 0x00,  //3
0xfc, 0x04, 0x04, 0x1e, 0x04, 0x04, 0x00, 0x00,  //4
0xf2, 0x92, 0x92, 0x92, 0x92, 0x0c, 0x00, 0x00,  //5
0xfc, 0x12, 0x12, 0x12, 0x12, 0x12, 0x0c, 0x00,  //6
0x80, 0x80, 0x80, 0x86, 0x8c, 0xb0, 0xc0, 0x00,  //7
0x6c, 0x92, 0x92, 0x92, 0x92, 0x92, 0x6c, 0x00,  //8
0x60, 0x90, 0x90, 0x90, 0x90, 0x90, 0xfe, 0x00,  //9
};