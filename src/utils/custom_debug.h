#ifndef _CUSTOM_DEBUG_
#define _CUSTOM_DEBUG_

#ifndef D_PRINTF
#  if defined(USE_D_PRINTF) && USE_D_PRINTF
#    define D_PRINTF(fmt, ...) printf(fmt, ##__VA_ARGS__)
#  else
#    define D_PRINTF(fmt, ...) (void)0
#  endif
#endif

#endif /* _CUSTOM_DEBUG_ */