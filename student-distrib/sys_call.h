
#ifndef _SYS_CALL_H
#define _SYS_CALL_H

#include "types.h"

/* number of possible files/devices we can have open */
#define MAX_FILE 		8

/* strings used for specific devices */
#define RTC_NAME 		"rtc"

/* the entry to the file array filled on a "open" system call */
typedef struct file_array_entry {
	int32_t (*dev_open)(const uint8_t* filename);
	int32_t (*dev_close)(int32_t fd);
	int32_t (*dev_read)(int32_t fd, void* buf, int32_t nbytes);
	int32_t (*dev_write)(int32_t fd, const void* buf, int32_t nbytes);
	uint32_t inode;
	uint32_t file_pos;
	uint32_t present;
} file_array_entry;

/* file array holding information about any open device, directory, or file */
file_array_entry file_array[MAX_FILE];

/* the general system call function that determines the type of call and performs the necessary operation */
int32_t do_sys_call(int call_num, uint32_t arg1, uint32_t arg2, uint32_t arg3);

/* the subfunctions that performs the specified system call */
int32_t __sys_open__(const uint8_t* filename);
int32_t __sys_close__(int32_t fd);
int32_t __sys_read__(int32_t fd, void* buf, int32_t nbytes);
int32_t __sys_write__(int32_t fd, const void* buf, int32_t nbytes);

#endif /* _SYS_CALL_H */
