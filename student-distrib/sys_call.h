
#ifndef _SYS_CALL_H
#define _SYS_CALL_H

#include "types.h"

/* number of possible files/devices we can have open */
#define MAX_FILE 			8

/* arguments buffer size */
#define ARG_BUF_SIZE		1024

/* strings used for specific devices */
#define RTC_NAME 			"rtc"
#define RTC_NAME_LEN 		4
#define TERMINAL_NAME		"terminal"
#define TERMINAL_NAME_LEN 	9

/* the entry to the file array filled on a "open" system call */
typedef struct file_array_entry {
	int32_t (*dev_open)(const uint8_t* filename);
	int32_t (*dev_close)(int32_t fd);
	int32_t (*dev_read)(int32_t fd, void* buf, int32_t nbytes);
	int32_t (*dev_write)(int32_t fd, const void* buf, int32_t nbytes);
	uint32_t inode;
	uint32_t file_pos;
	uint32_t present;
} file_array_entry_t;

/* file array holding information about any open device, directory, or file */
file_array_entry_t file_array[MAX_FILE];

/* the subfunctions that performs the specified system call */
int32_t sys_halt(uint8_t status);
int32_t sys_execute(const uint8_t* command);
int32_t sys_read(int32_t fd, void* buf, int32_t nbytes);
int32_t sys_write(int32_t fd, const void* buf, int32_t nbytes);
int32_t sys_open(const uint8_t* filename);
int32_t sys_close(int32_t fd);
int32_t sys_getargs(uint8_t* buf, int32_t nbytes);
int32_t sys_vidmap(uint8_t** screen_start);
int32_t set_handler(int32_t signum, void* handler_address);
int32_t sigreturn(void);

#endif /* _SYS_CALL_H */
