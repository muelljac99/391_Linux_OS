
#include "multiboot.h"
#include "x86_desc.h"
#include "lib.h"
#include "i8259.h"
#include "irq_asm.h"
#include "service_irq.h"
#include "sys_call.h"
#include "rtc.h"
#include "terminal.h"

#define EXCEPTION_STATUS			256

const char exception_resp[NUM_EXCEPTION][32] = {"Division by Zero", "Single-step Interrupt", "Non-Maskablee Interrupt",
												"Breakpoint", "Overflow", "Bounds", "Invalid Opcode", "Coprocessor N/A",
												"Double Fault", "Coprocessor Seg Over", "Invalid Task State Seg", "Segment Not Present",
												"Stack Fault", "General Protection Fault", "Page Fault", "reserved",
												"Math Fault", "Alignment Check", "Machine Check", "SIMD Float Exception",
												"Virtualization Exception", "Control Protection Exception", "Intel reserved",
												"Intel reserved", "Intel reserved", "Intel reserved", "Intel reserved",
												"Intel reserved", "Intel reserved", "Intel reserved", "Intel reserved",
												"Intel reserved"};

/* 
 * do_irq
 *   DESCRIPTION: This is the common C function for all interrupts. It takes in the complemented irq number and
 * 				  determines the proper execution based on the type of irq and the handler function if it is an interrupt.
 *   INPUTS: a ptr to saved registers as well as the irq number
 *   OUTPUTS: none
 *   RETURN VALUE: 0 on success, -1 on failure
 *   SIDE EFFECTS: masks and unmasks interrupt lines if interrupt and freezes if an exception
 */
int32_t do_irq(pt_regs_t* reg){
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
		//clear();
		printf("EXCEPTION #%x: %s\n", irq, exception_resp[irq]);
		__sys_halt(EXCEPTION_STATUS);
		//while(1);
	}
	else if(irq == SYS_CALL_IRQ){
		// shouldn't get to the common_irq from int 0x80 so this is an error
		printf("SYSTEM CALL ASSEMBLY LINKAGE ERROR");
		return -1;
	}
	else if(irq >= BASE_INT && irq <= PIC_IRQ_MAX){
		// this is a PIC interrupt
		disable_irq(irq);							//mask irq
		send_eoi(irq);								//send eoi
		(*irq_handlers[(irq&0x0F)])();				//call handler function
		enable_irq(irq);							//unmask irq
	}
	else{
		// the rest are unused interrupts
		printf("EXTRA INTERRUPT #%x\n", irq);
	}
	restore_flags(flags);
	return 0;
}
