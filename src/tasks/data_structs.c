#include "data_structs.h"

uint calculate_IC_Control_Data_size(IC_Control_Data *iccd) {
    return sizeof(IC_Control_Data) + (iccd->address_len) + (iccd->data_len) + (iccd->power_len) + (iccd->special_len*2);
}
