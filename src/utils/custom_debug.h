#ifndef _CUSTOM_DEBUG_
#define _CUSTOM_DEBUG_

#include "pico/stdio/driver.h"
#include "pico/stdio_usb.h"
#include "pico/stdio_uart.h"


void init_print(void);

int retarg_printf(const stdio_driver_t *driver, const char *fmt, ...);

int __attribute__((weak)) dbg_printf(const char *func, int line, const char *fmt, ...);

#ifndef D_PRINTF
#  if defined(USE_D_PRINTF) && USE_D_PRINTF
#    define D_PRINTF(fmt, ...) dbg_printf(__func__, __LINE__, fmt, ##__VA_ARGS__)
#  else
#    define D_PRINTF(fmt, ...) (void)0
#  endif
#endif

#ifndef DD_PRINTF
#  if DEBUG == 1
#    define DD_PRINTF(fmt, ...) D_PRINTF(fmt, ##__VA_ARGS__)
#  else
#    define DD_PRINTF(fmt, ...) (void)0
#  endif
#endif

#ifndef USB_PRINTF
#  define USB_PRINTF(fmt, ...) retarg_printf(&stdio_usb, fmt, ##__VA_ARGS__)
#endif

#endif /* _CUSTOM_DEBUG_ */