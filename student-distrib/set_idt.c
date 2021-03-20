
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
			idt[idt_index].present = 0;
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
			idt[idt_index].present = 0;
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
