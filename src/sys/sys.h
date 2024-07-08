#ifndef _SYS_HEADER_
#define _SYS_HEADER_

#define LED_GPIO 25

#define RELAY_ENABLE_GPIO 28

#define SIPO_OE_GPIO 21
#define SIPO_SER_GPIO 20
#define SIPO_CLK_GPIO 19
#define SIPO_CLR_GPIO 18
#define SIPO_RCLK_GPIO 17

#define PISO_SER_GPIO 27
#define PISO_SH_GPIO 2
#define PISO_CLK_GPIO 3
#define PISO_INH_GPIO 4
#define PISO_CLR_GPIO 5

void sys_init(void);

#endif /* _SYS_HEADER_ */