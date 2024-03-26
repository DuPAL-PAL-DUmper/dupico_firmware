#ifndef _CUSTOM_DEBUG_
#define _CUSTOM_DEBUG_

int __attribute__((weak)) dbg_printf(const char *func, int line, const char *fmt, ...);

#ifndef D_PRINTF
#  if defined(USE_D_PRINTF) && USE_D_PRINTF
#    define D_PRINTF(fmt, ...) dbg_printf(__func__, __LINE__, fmt, ##__VA_ARGS__)
#  else
#    define D_PRINTF(fmt, ...) (void)0
#  endif
#endif

#endif /* _CUSTOM_DEBUG_ */