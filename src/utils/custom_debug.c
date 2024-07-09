#include "custom_debug.h" 

#include <stdio.h>
#include <stdarg.h>

#include "pico/stdio_uart.h"
#include "pico/printf.h"

void out_char_driver(char c, void *arg) {
  ((stdio_driver_t *)arg)->out_chars(&c, 1);
}

int retarg_printf(const stdio_driver_t *driver, const char *fmt, ...) {
  va_list va;
  va_start(va, fmt);
  int ret = vfctprintf(out_char_driver, (void *)driver, fmt, va);
  va_end(va);

  return ret;
}

int __attribute__((weak)) dbg_printf(const char *func, int line, const char *fmt, ...) {
    retarg_printf(&stdio_uart, "%s:%d: ", func, line);

    va_list va;
    va_start(va, fmt);
    int ret = vfctprintf(out_char_driver, (void *)&stdio_uart, fmt, va);
    va_end(va);

    return ret;
}