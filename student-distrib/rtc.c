
#include "multiboot.h"
#include "x86_desc.h"
#include "lib.h"
#include "i8259.h"
#include "irq_asm.h"
#include "service_irq.h"
#include "sys_call.h"
#include "sys_call_asm.h"
#include "rtc.h"

/* flag telling when the rtc interrupt has occurred */
volatile int rtc_flag = 0;

/* current count value for the rtc */
int rtc_count = 0;

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
	pcb_t* curr_pcb = (pcb_t*)(get_esp()&PCB_MASK);
	//test_interrupts();
	
	if(curr_pcb->rtc_freq != 0){
		if(rtc_count++ >= (RTC_RATE/(curr_pcb->rtc_freq))){
			//only set the interrupt flag once the counter has reached the target
			rtc_count = 0;
			rtc_flag = 0;
		}
	}
	
	outb(RTC_REGC, RTC_PORT);			//select the register C
	inb(RTC_PORT+1); 				//read it so the next interrupts will come
}

/* 
 * rtc_open
 *   DESCRIPTION: The rtc open driver function. This function will set the frequency of the rtc interrupt
 * 				  to 2Hz as a default.
 *   INPUTS: filename -- unused
 *   OUTPUTS: none
 *   RETURN VALUE: 0 on success, -1 on failure
 *   SIDE EFFECTS: none
 */
int32_t rtc_open(const uint8_t* filename){
	pcb_t* curr_pcb = (pcb_t*)(get_esp()&PCB_MASK);
	unsigned int flags;
	
	//start of critical section
	cli_and_save(flags);
	
	//set the virtual rate for the process to 2
	curr_pcb->rtc_freq = 2;
	
	restore_flags(flags);
	return 0;
}

/* 
 * rtc_close
 *   DESCRIPTION: The rtc close driver function. This function sets the virtual rtc rate back to 0
 *   INPUTS: fd -- unused
 *   OUTPUTS: none
 *   RETURN VALUE: 0 on success, -1 on failure
 *   SIDE EFFECTS: none
 */
int32_t rtc_close(int32_t fd){
	pcb_t* curr_pcb = (pcb_t*)(get_esp()&PCB_MASK);
	unsigned int flags;
	
	//start of critical section
	cli_and_save(flags);
	
	//set the virtual rate for the process to 2
	curr_pcb->rtc_freq = 0;
	
	restore_flags(flags);
	return 0;
}

/* 
 * rtc_read
 *   DESCRIPTION: The rtc read driver function. This function will wait for an rtc interrupt and
 * 				  return once one has been received.
 *   INPUTS: fd -- unused
 * 			 buf -- unused
 * 			 nbytes -- unused
 *   OUTPUTS: none
 *   RETURN VALUE: 0 on success, -1 on failure
 *   SIDE EFFECTS: changes the volatile rtc_flag
 */
int32_t rtc_read(int32_t fd, void* buf, int32_t nbytes){
	rtc_flag = 1;
	//wait until rtc interrupt returns the flag to 0
	while(rtc_flag != 0);
	return 0;
}


/* 
 * rtc_open
 *   DESCRIPTION: The rtc write driver function. This function will set the frequency of the rtc interrupt
 * 				  to a specified power of 2 value.
 *   INPUTS: fd -- unused
 * 			 buf -- a pointer to an int that holds the desired frequency
 * 			 nbytes -- unused
 *   OUTPUTS: none
 *   RETURN VALUE: 0 on success, -1 on failure
 *   SIDE EFFECTS: writes to the rtc registers
 */
int32_t rtc_write(int32_t fd, const void* buf, int32_t nbytes){
	pcb_t* curr_pcb = (pcb_t*)(get_esp()&PCB_MASK);
	int32_t* freq_ptr = (int32_t*)buf;
	int32_t freq;
	unsigned int flags;
	
	//check that its not a NULL ptr
	if(freq_ptr == NULL){
		return -1;
	}
	else{
		freq = (*freq_ptr);
	}
	
	if(freq <= 1 || freq > 1024){
		//frequency is out of bounds (max is 1024 Hz)
		return -1;
	}
	// this is a power of 2
	cli_and_save(flags);

	curr_pcb->rtc_freq = freq;
	
	restore_flags(flags);
	return 0;
}
