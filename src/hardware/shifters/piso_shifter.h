#ifndef _PISO_SHIFTER_HEADER_
#define _PISO_SHIFTER_HEADER_

#include <stdint.h>
#include <sys/types.h>

// Driver for 74'166 parallel-load shift register

typedef struct {
    const uint ce_pin; // Clock inhibit
    const uint pe_pin; // Shift/Load pin
    const uint clk_pin; // Clock pin
    const uint clr_pin; // Clear pin
    const uint ser_pin; // Serial out pin (our input)
    const uint8_t len;
} PISO_Config;

void piso_shifter_init(PISO_Config* cfg);
uint64_t piso_shifter_get(PISO_Config* cfg);

#endif /* _PISO_SHIFTER_HEADER_ */