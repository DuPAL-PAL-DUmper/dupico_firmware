#include "data_structs.h"

uint calculate_IC_Ctrl_Struct_size(IC_Ctrl_Struct *iccd) {
    return sizeof(IC_Ctrl_Struct) + (iccd->address_len) + (iccd->data_len) + (iccd->power_len) + (iccd->special_len*2);
}
