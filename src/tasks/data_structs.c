#include "data_structs.h"

uint calculate_IC_Ctrl_Struct_size(IC_Ctrl_Struct *iccd) {
    return sizeof(IC_Ctrl_Struct) + (iccd->i_len) + (iccd->io_len) + (iccd->pwr_len) + (iccd->ctrl_len*2);
}
