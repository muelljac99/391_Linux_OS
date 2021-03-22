
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
	}
	else{
		// the rest are unused interrupts
		printf("EXTRA INTERRUPT #%x\n", irq);
	}
	restore_flags(flags);
	return 0;
}

void handle_keyboard(void){
	char scancode[] = {0 , 0, '1', '2', '3','4', '5', '6', '7', '8', '9', '0', 0, 0, 0, 0, 'q', 'w', 'e', 'r', 't', 'y', 'u', 'i',
					   'o', 'p', 0, 0, 0, 0, 'a', 's', 'd', 'f', 'g', 'h', 'j', 'k', 'l', 0, 0, 0, 0, 0, 'z', 'x', 'c', 'v', 'b', 'n', 
					   'm', ',', '.', '/', 0, 0, 0, ' '};
	uint32_t scan;
	scan = inb(KEYBOARD_PORT);
	// 0x40 is the number of elements that we have characters chosen for
	if(scan < 0x40 && scancode[scan] != 0){
		putc(scancode[scan]);
	}
	return;
}

void handle_rtc(void){
	printf("RTC");
	
	outb(RTC_PORT, 0x0C);			//select the register C
	inb(RTC_PORT+1); 				//read it so the next interrupts will come
}

void init_rtc(void){
	unsigned int flags;
	char regB;
	
	// handle our side of the initialization for the PIC and IDT
	idt[RTC_IRQ].present = 1;
	irq_handlers[RTC_IRQ - BASE_INT] = handle_rtc;
	enable_irq(RTC_IRQ);
	
	//start of critical section
	cli_and_save(flags);
	
	outb(RTC_PORT, 0x8A);		//select status register A and disable NMI
	outb(RTC_PORT+1, 0x20);		//write to the RTC RAM
	
	outb(RTC_PORT, 0x8B);		//select status register B and disable NMI
	regB = inb(RTC_PORT+1);		//read the value in regB
	outb(RTC_PORT, 0x8B);		//select the status register B and disable NMI again
	outb(RTC_PORT+1, (regB|0x40));	//turn on bit 6 of the register but keep the rest the same
	
	// we do not want to change the interrupt rate
	return;
}
