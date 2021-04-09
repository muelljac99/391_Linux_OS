
#ifndef _SYS_CALL_H
#define _SYS_CALL_H

#include "types.h"

/* number of possible files and processes we can have open */
#define MAX_FILE 			8
#define MAX_PROCESS			6

/* PCB constants */
#define PCB_MASK 			0xFFFFE000
#define KERNEL_BASE			0x00800000
#define KERNEL_STACK		0x00002000

/* user process page info */
#define USER_PAGE_IDX		32
#define USER_PROG_IMG		0x08048000

/* arguments buffer size */
#define ARG_BUF_SIZE		1024

/* strings used for specific devices */
#define RTC_NAME 			"rtc"
#define RTC_NAME_LEN 		4
#define TERMINAL_NAME		"terminal"
#define TERMINAL_NAME_LEN 	9

#ifndef ASM

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

typedef struct pcb {
	file_array_entry_t file_array[MAX_FILE];
	uint32_t parent_process_num;
	struct pcb* parent_pcb_ptr;
	uint32_t parent_esp0;
	uint16_t parent_ss0;
} pcb_t;

/* array of flags indicating if a process number is available */
uint32_t process_present[MAX_PROCESS];

/* the subfunctions that performs the specified system call */
int32_t sys_halt(uint8_t status);
int32_t sys_execute(const uint8_t* command);
int32_t sys_read(int32_t fd, void* buf, int32_t nbytes);
int32_t sys_write(int32_t fd, const void* buf, int32_t nbytes);
int32_t sys_open(const uint8_t* filename);
int32_t sys_close(int32_t fd);
int32_t sys_getargs(uint8_t* buf, int32_t nbytes);
int32_t sys_vidmap(uint8_t** screen_start);
int32_t sys_sethandler(int32_t signum, void* handler_address);
int32_t sys_sigreturn(void);

/* check if file is executable helper function */
int32_t exe_check(uint8_t* name_buf);

#endif /* ASM */

#endif /* _SYS_CALL_H */
