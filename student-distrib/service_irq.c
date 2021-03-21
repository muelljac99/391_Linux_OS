
#include "multiboot.h"
#include "x86_desc.h"
#include "lib.h"
#include "i8259.h"
#include "irq_asm.h"
#include "service_irq.h"

/* do_irq
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
	else if(irq >= BASE_INT && irq <= PIC_IRQ_MAX){
		// this is a PIC interrupt
		disable_irq(irq);							//mask irq
		send_eoi(irq);								//send eoi
		(*irq_handlers[(irq&0x0F)])();				//call handler function
		enable_irq(irq);							//unmask irq
		printf("PIC INTERRUPT #%x\n", irq);
	}
	else{
		// the rest are unused interrupts
		printf("EXTRA INTERRUPT #%x\n", irq);
	}
	restore_flags(flags);
	return 0;
}

void handle_keyboard(void){

}
