#ifndef _BINUTILS_HEADER_
#define _BINUTILS_HEADER_

#include <stdint.h>

uint64_t binutils_read_u64_le(uint8_t *buf);
void binutils_write_u64_le(uint8_t *buf, uint64_t data);

#endif /* _BINUTILS_HEADER_ */