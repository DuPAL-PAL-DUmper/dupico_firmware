#ifndef _BASIC_HANDLERS_IO_HEADER_
#define _BASIC_HANDLERS_IO_HEADER_

#include <stdint.h>

#include <tasks/data_structs.h>
#include <tasks/ic_interfacer_task.h>

void handler_write_inputs(const IC_Ctrl_Struct *ic_ctrl, const ic_interfacer_task_params *interfacer_params, uint32_t inputs);
void handler_write_io(const IC_Ctrl_Struct *ic_ctrl, const ic_interfacer_task_params *interfacer_params, uint16_t io);
uint16_t handler_read_io(const IC_Ctrl_Struct *ic_ctrl, const ic_interfacer_task_params *interfacer_params);
void handler_write_power(const IC_Ctrl_Struct *ic_ctrl, const ic_interfacer_task_params *interfacer_params, uint8_t power);
void handler_write_control(const IC_Ctrl_Struct *ic_ctrl, const ic_interfacer_task_params *interfacer_params, uint8_t controls);

#endif /* _BASIC_HANDLERS_IO_HEADER_ */
