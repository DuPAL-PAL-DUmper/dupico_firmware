#ifndef _STRUTILS_HEADER_
#define _STRUTILS_HEADER_

#include <stdint.h>

uint8_t strutils_str_to_u8(char *str);
uint32_t strutils_str_to_u32(char *str);
uint64_t strutils_str_to_u64(char *str);
void strutils_u8_to_str(char *str, uint8_t data);
void strutils_u32_to_str(char *str, uint32_t data);
void strutils_u64_to_str(char *str, uint64_t data);

#endif /* _STRUTILS_HEADER_ */