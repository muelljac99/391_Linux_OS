
#include "multiboot.h"
#include "x86_desc.h"
#include "lib.h"
#include "i8259.h"
#include "irq_asm.h"
#include "service_irq.h"
#include "sys_call.h"
#include "terminal.h"

int32_t terminal_open(const uint8_t* filename){
	return 0;
}

int32_t terminal_close(int32_t fd){
	return 0;
}

int32_t terminal_read(int32_t fd, void* buf, int32_t nbytes){
	return 0;
}

int32_t terminal_write(int32_t fd, const void* buf, int32_t nbytes){
	return 0;
}
