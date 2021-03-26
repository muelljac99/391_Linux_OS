
#include "multiboot.h"
#include "x86_desc.h"
#include "lib.h"
#include "i8259.h"
#include "irq_asm.h"
#include "service_irq.h"
#include "sys_call.h"
#include "rtc.h"

/* flag telling when the rtc interrupt has occurred */
int rtc_flag = 0;

/* 
 * init_rtc
 *   DESCRIPTION: Initialize the rtc by setting the idt entry accordingly and unmasking the irq line. The NMI is
 * 				  disabled to set up the device itself
 *   INPUTS: none
 *   OUTPUTS: none
 *   RETURN VALUE: none
 *   SIDE EFFECTS: changes the idt and writes to rtc registers
 */
void init_rtc(void){
	unsigned int flags;
	char regB;
	
	// handle our side of the initialization for the PIC and IDT
	idt[RTC_IRQ].present = 1;
	irq_handlers[RTC_IRQ - BASE_INT] = handle_rtc;
	
	//start of critical section
	cli_and_save(flags);
	
	outb(RTC_NMI_REGB, RTC_PORT);		//select status register B and disable NMI
	regB = inb(RTC_PORT+1);				//read the value in regB
	outb(RTC_NMI_REGB, RTC_PORT);		//select the status register B and disable NMI again
	outb((regB|0x40), RTC_PORT+1);		//turn on bit 6 of the register but keep the rest the same
	
	// set the frequency to the default rate
	outb(RTC_NMI_REGA, RTC_PORT);
	outb(0x26, RTC_PORT+1); 			//0x26 is the default rtc frequency and rate setting
	
	enable_irq(RTC_IRQ);
	
	restore_flags(flags);
	return;
}

/* 
 * handle_rtc
 *   DESCRIPTION: The handler function for the rtc. Calls the test_interrupts function and clears out the
 * 				  C register to ensure more interrupts from the rtc can follow
 *   INPUTS: none
 *   OUTPUTS: none
 *   RETURN VALUE: none
 *   SIDE EFFECTS: clears the C register on the rtc
 */
void handle_rtc(void){
	//test_interrupts();
	
	// set the interrupt flag to 0
	rtc_flag = 0;
	
	outb(RTC_REGC, RTC_PORT);			//select the register C
	inb(RTC_PORT+1); 				//read it so the next interrupts will come
}

int32_t rtc_open(const uint8_t* filename){
	unsigned int flags;
	
	//start of critical section
	cli_and_save(flags);
	
	// set the frequency to 2Hz
	outb(RTC_NMI_REGA, RTC_PORT);
	outb(0x2F, RTC_PORT+1); 			// the lower 4 bits is the divider value and F corresponds to 2 Hz
	
	restore_flags(flags);
	return 0;
}

int32_t rtc_close(int32_t fd){
	//does nothing
	return 0;
}

int32_t rtc_read(int32_t fd, int* buf, int32_t nbytes){
	rtc_flag = 1;
	//wait until rtc interrupt returns the flag to 0
	while(rtc_flag != 0);
	return 0;
}

int32_t rtc_write(int32_t fd, const int* buf, int32_t nbytes){
	int32_t freq;
	unsigned int flags;
	int match = 2;
	char rate = 0x0F;
	
	//check that its not a NULL ptr
	if(buf == NULL){
		return -1;
	}
	else{
		freq = (int32_t)(*buf);
	}
	
	if(freq <= 1 || freq > 1024){
		//frequency is out of bounds
		return -1;
	}
	else if((freq&(freq-1)) == 0){
		// find the rate corresponding to this power of 2 frequency
		while(freq != match){
			match = match << 1;
			rate--;
		}
		// this is a power of 2
		cli_and_save(flags);
	
		// set the frequency to 2Hz
		outb(RTC_NMI_REGA, RTC_PORT);
		outb((0x20|rate), RTC_PORT+1); 			// the lower 4 bits is the divider value and F corresponds to 2 Hz
		
		restore_flags(flags);
		return 0;
	}
	else{
		return -1;
	}
}
