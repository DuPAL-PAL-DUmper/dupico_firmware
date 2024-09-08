#include "custom_debug.h" 

#include <stdio.h>
#include <stdarg.h>

#include <pico/stdio_uart.h>
#include <pico/printf.h>

#include <FreeRTOS.h>
#include <semphr.h>

static SemaphoreHandle_t xSerialPrintSemaphore;
static SemaphoreHandle_t xUSBPrintSemaphore;

void init_print(void) {
  xSerialPrintSemaphore = xSemaphoreCreateMutex();
  xUSBPrintSemaphore = xSemaphoreCreateMutex();
  // We'll pretend this goes well, if it does not, we're in deep trouble anyway
}

void out_char_driver(char c, void *arg) {
  ((stdio_driver_t *)arg)->out_chars(&c, 1);
}

int retarg_printf(const stdio_driver_t *driver, const char *fmt, ...) {
  va_list va;
  
  SemaphoreHandle_t sem = driver == &stdio_usb ? xUSBPrintSemaphore : xSerialPrintSemaphore;

  xSemaphoreTake(sem, portMAX_DELAY);

  va_start(va, fmt);
  int ret = vfctprintf(out_char_driver, (void *)driver, fmt, va);
  va_end(va);

  xSemaphoreGive(sem);
  
  return ret;
}

int __attribute__((weak)) dbg_printf(const char *func, int line, const char *fmt, ...) {
    retarg_printf(&stdio_uart, "%s:%d: ", func, line);

    va_list va;

    xSemaphoreTake(xSerialPrintSemaphore, portMAX_DELAY);

    va_start(va, fmt);
    int ret = vfctprintf(out_char_driver, (void *)&stdio_uart, fmt, va);
    va_end(va);

    stdio_flush();

    xSemaphoreGive(xSerialPrintSemaphore);

    return ret;
}