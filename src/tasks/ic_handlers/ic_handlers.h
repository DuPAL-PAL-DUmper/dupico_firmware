#ifndef _IC_HANDLERS_HEADER_
#define _IC_HANDLERS_HEADER_

#include <stdint.h>
#include <pico/types.h>

#include <tasks/data_structs.h>

typedef void* (*exec_command)(uint, IC_Ctrl_Struct*, const void*);

#endif /* _IC_HANDLERS_HEADER_ */