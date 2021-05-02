
#include "multiboot.h"
#include "x86_desc.h"
#include "lib.h"
#include "i8259.h"
#include "irq_asm.h"
#include "service_irq.h"
#include "sys_call.h"
#include "sys_call_asm.h"
#include "pit.h"
#include "scheduler.h"

void init_pit(void){
	uint32_t flags;
	// handle our side of the initialization for the PIC and IDT
	idt[PIT_IRQ].present = 1;
	irq_handlers[PIT_IRQ - BASE_INT] = handle_pit;
	
	cli_and_save(flags);
	
	//set the PIT to operate as a rate generator
	outb(PIT_CTRL_WORD, PIT_CMD);
	
	//set the reload rate to about 25ms interrupts
	outb(PIT_LOBYTE, PIT_DATA);
	outb(PIT_HIBYTE, PIT_DATA);
	
	//enable PIC interrupts for the PIT
	enable_irq(PIT_IRQ);
	
	restore_flags(flags);
}

void handle_pit(void){
	//call the scheduler function
	scheduler();
}

