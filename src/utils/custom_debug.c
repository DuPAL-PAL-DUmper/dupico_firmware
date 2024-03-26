#include "custom_debug.h" 

#include <stdio.h>
#include <stdarg.h>

int __attribute__((weak)) dbg_printf(const char *func, int line, const char *fmt, ...) {
    printf("%s:%d: ", func, line);
    va_list args;
    va_start(args, fmt);
    int cw = vprintf(fmt, args);
    va_end(args);
    fflush(stdout);
    return cw;
}