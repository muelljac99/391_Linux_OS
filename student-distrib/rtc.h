
#ifndef _RTC_H
#define _RTC_H

#include "types.h"

/* the rtc irq and port */
#define RTC_IRQ 		0x28
#define RTC_PORT 		0x70

/* rtc register access */
#define RTC_NMI_REGA 	0x8A
#define RTC_NMI_REGB 	0x8B
#define RTC_REGC		0x0C

/* initializing the rtc */
void init_rtc(void);

/* handler for the rtc */
void handle_rtc(void);

int32_t rtc_open(const uint8_t* filename);

int32_t rtc_close(int32_t fd);

int32_t rtc_read(int32_t fd, int* buf, int32_t nbytes);

int32_t rtc_write(int32_t fd, const int* buf, int32_t nbytes);

#endif /* _RTC_H */