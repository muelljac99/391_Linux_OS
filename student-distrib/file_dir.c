
#include "multiboot.h"
#include "x86_desc.h"
#include "lib.h"
#include "i8259.h"
#include "irq_asm.h"
#include "service_irq.h"
#include "sys_call.h"
#include "file_dir.h"

int32_t file_open(const uint8_t* filename){
	return 0;
}

int32_t file_close(int32_t fd){
	return 0;
}

int32_t file_read(int32_t fd, void* buf, int32_t nbytes){
	return 0;
}

int32_t file_write(int32_t fd, const void* buf, int32_t nbytes){
	return 0;
}

int32_t dir_open(const uint8_t* filename){
	return 0;
}

int32_t dir_close(int32_t fd){
	return 0;
}

int32_t dir_read(int32_t fd, void* buf, int32_t nbytes){
	return 0;
}

int32_t dir_write(int32_t fd, const void* buf, int32_t nbytes){
	return 0;
}
