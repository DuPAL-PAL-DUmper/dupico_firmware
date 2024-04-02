#include "ic_handlers.h"

#include "unknown_ic_handler.h"

static const handlers_getter handler_arr[] = {

};

const handler_funcs get_handlers_for_IC_type(uint16_t ic_type) {
    if(ic_type == 0xFFFF) return build_unknown_ic_handler();

    return handler_arr[ic_type](); 
}