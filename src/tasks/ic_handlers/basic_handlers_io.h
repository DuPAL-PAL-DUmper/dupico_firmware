#ifndef _BASIC_HANDLERS_IO_HEADER_
#define _BASIC_HANDLERS_IO_HEADER_

#include <stdint.h>

#include <tasks/data_structs.h>
#include <tasks/ic_interfacer_task.h>

void handler_set_inputs(const ic_interfacer_task_params *interfacer_params, uint32_t inputs);
void handler_set_io(const ic_interfacer_task_params *interfacer_params, uint16_t io);
uint16_t handler_get_io(const ic_interfacer_task_params *interfacer_params);
void handler_set_power(const ic_interfacer_task_params *interfacer_params, uint8_t power);
void handler_set_control(const ic_interfacer_task_params *interfacer_params, uint8_t controls);
void handler_commit(const ic_interfacer_task_params *interfacer_params);

#endif /* _BASIC_HANDLERS_IO_HEADER_ */
