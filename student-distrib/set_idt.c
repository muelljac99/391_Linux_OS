
#include "multiboot.h"
#include "x86_desc.h"
#include "lib.h"
#include "i8259.h"
#include "set_idt.h"
#include "irq_asm.h"

/*	idt_init
 *
 *
 */
void idt_fill(void){
	/* Fill all the entries of the IDT with NULL handlers and present = 0 */
	lidt(idt_desc_ptr);  // initialize the idtr
	int idt_index;
	for(idt_index = 0; idt_index < NUM_VEC; idt_index++){
		// fill the exception IRQ descriptors
		if(idt_index < BASE_INT){
			SET_IDT_ENTRY(idt[idt_index], irq_handle[idt_index]);
			idt[idt_index].present = 1;
			idt[idt_index].seg_selector = KERNEL_CS;
			idt[idt_index].dpl = 0;
			idt[idt_index].reserved3 = 0;
		}
		// fill the interrupt IRQ descriptors
		else if(idt_index != SYS_CALL_IRQ){
			SET_IDT_ENTRY(idt[idt_index], irq_handle[idt_index]);
			idt[idt_index].present = 0;
			idt[idt_index].seg_selector = KERNEL_CS;
			idt[idt_index].dpl = 0;
			idt[idt_index].reserved3 = 0;
		}
		// fill the system call IRQ descriptor
		else{
			SET_IDT_ENTRY(idt[idt_index], irq_handle[idt_index]);
			idt[idt_index].present = 1;
			idt[idt_index].seg_selector = KERNEL_CS;
			idt[idt_index].dpl = 3;
			//system call has reserve3 of 1
			idt[idt_index].reserved3 = 1;
		}
		// all the IRQs have a size of 32 bits
		idt[idt_index].size = 1;
		// all IRQs have the same following reserve vals
		idt[idt_index].reserved4 = 0;
		idt[idt_index].reserved2 = 1;
		idt[idt_index].reserved1 = 1;
		idt[idt_index].reserved0 = 0;
	}
	
	// set the handlers for the exceptions and the interrupts we want at boot
	
}

/*
 *
 */
uint32_t do_irq(pt_regs_t* reg){
	unsigned int irq = ~(reg->irq_num);
	unsigned int flags;
	
	//critical section starts here
	cli_and_save(flags);
	// invalid irq #
	if(irq >= NUM_VEC){
		printf("INVALID IRQ #%x: CANNOT HANDLE\n", irq);
		return -1;
	}
	// check that the idt entry is present
	if(idt[irq].present == 0){
		printf("IRQ ENTRY NOT INITIALIZED: IRQ #%x\n", irq);
		return -1;
	}
	
	// check the type of irq
	if(irq < BASE_INT){
		// this is an exception
		printf("EXCEPTION #%x\n", irq);
		while(1);
	}
	else if(irq == SYS_CALL_IRQ){
		// this is a system call
		printf("SYSTEM CALL\n");
	}
	else{
		//anything else is an interrupt
		printf("INTERRUPT #%x\n", irq);
	}
	restore_flags(flags);
	return 0;
	
}
