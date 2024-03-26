#ifndef _SIPO_SHIFTER_HEADER_
#define _SIPO_SHIFTER_HEADER_

#include <stdint.h>
#include <sys/types.h>

// Driver for 74'595 parallel-load shift register

typedef struct {
    const uint oe_pin; // /OE
    const uint ser_pin; // Serial input (an output for us)
    const uint srclk_pin; // Shift clock pin, causes stages to store the data of previous stage
    const uint rclk_pin; // Shift-register data is stored in the storage register
    const uint srclr_pin; // Clear pin
    const uint8_t len;
} SIPO_Config;

void sipo_shifter_init(const SIPO_Config* cfg);
void sipo_shifter_set(const SIPO_Config* cfg, uint64_t val);

#endif /* _SIPO_SHIFTER_HEADER_ */