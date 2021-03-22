/* i8259.c - Functions to interact with the 8259 interrupt controller
 * vim:ts=4 noexpandtab
 */

#include "i8259.h"
#include "lib.h"

/* Interrupt masks to determine which interrupts are enabled and disabled */
uint8_t master_mask; /* IRQs 0-7  */
uint8_t slave_mask;  /* IRQs 8-15 */

/* Initialize the 8259 PIC */
void i8259_init(void) {
	unsigned int flags;
	int i;
	//save the flags and start critical section
	cli_and_save(flags);
	
	//mask the interrupts on both PICs
	master_mask = 0xFF;
	slave_mask = 0xFF;
	outb(master_mask, MASTER_8259_PORT+1); 		// +1 corresponds to the data port
	outb(slave_mask, SLAVE_8259_PORT+1);

	//send the master and slave initialization words (no access to an outb_p macro)
	outb(ICW1, MASTER_8259_PORT);
	outb(ICW1, SLAVE_8259_PORT);

	outb(ICW2_MASTER, MASTER_8259_PORT+1);
	outb(ICW2_SLAVE, SLAVE_8259_PORT+1);
	
	outb(ICW3_MASTER, MASTER_8259_PORT+1);
	outb(ICW3_SLAVE, SLAVE_8259_PORT+1);

	outb(ICW4, MASTER_8259_PORT+1);
	outb(ICW4, SLAVE_8259_PORT+1);
	
	//keep the irq lines masked and unmask at specific device initialization
	
	//restore the flags and end of critical section
	restore_flags(flags);
	
	enable_irq(0x22);		// enable the slave interrupt on the master
	
	return;
}

/* Enable (unmask) the specified IRQ */
void enable_irq(uint32_t irq_num) {
	uint8_t irq_bit = 0x01;
	unsigned int flags;
	
	irq_bit = irq_bit << (irq_num&0x07);			//shift according to which irq line we are on (0 to 7)
	irq_bit = ~(irq_bit);							//take complement of mask to have a 0 on the line we want to enable
	
	//start critical section while changing the interrupt masks
	cli_and_save(flags);
	
	if(irq_num >= MASTER_VECTOR && irq_num < SLAVE_VECTOR){
		//master PIC interrupt line
		master_mask = (master_mask&irq_bit);			//take AND of the master to preserve other masks
		outb(master_mask, MASTER_8259_PORT+1);
	}
	else if(irq_num >= SLAVE_VECTOR && irq_num <= PIC_IRQ_MAX){
		//slave PIC interrupt line
		slave_mask = (slave_mask&irq_bit);				//take AND of the slave to preserve other masks
		outb(slave_mask, SLAVE_8259_PORT+1);
	}
	else{
		printf("INVALID PIC IRQ NUMBER #%x: CANNOT ENABLE", irq_num);
	}
	
	//restore flags and leave critical section
	restore_flags(flags);
	return;
}

/* Disable (mask) the specified IRQ */
void disable_irq(uint32_t irq_num) {
	uint8_t irq_bit = 0x01;
	unsigned int flags;
	
	irq_bit = irq_bit << (irq_num&0x07);			//shift according to which irq line we are on (0 to 7)
	
	//start critical section while changing the interrupt masks
	cli_and_save(flags);
	
	if(irq_num >= MASTER_VECTOR && irq_num < SLAVE_VECTOR){
		//master PIC interrupt line
		master_mask = (master_mask|irq_bit);			//take OR of the master to preserve other masks
		outb(master_mask, MASTER_8259_PORT+1);
	}
	else if(irq_num >= SLAVE_VECTOR && irq_num <= PIC_IRQ_MAX){
		//slave PIC interrupt line
		slave_mask = (slave_mask|irq_bit);				//take OR of the slave to preserve other masks
		outb(slave_mask, SLAVE_8259_PORT+1);
	}
	else{
		printf("INVALID PIC IRQ NUMBER #%x: CANNOT DISABLE", irq_num);
	}
	
	//restore flags and leave critical section
	restore_flags(flags);
	return;
}

/* Send end-of-interrupt signal for the specified IRQ */
void send_eoi(uint32_t irq_num) {
	unsigned int flags;
	
	//start critical section while changing the interrupt masks
	cli_and_save(flags);
	
	if(irq_num >= MASTER_VECTOR && irq_num < SLAVE_VECTOR){
		//master PIC interrupt line
		outb((EOI|(irq_num&0x07)), MASTER_8259_PORT);
	}
	else if(irq_num >= SLAVE_VECTOR && irq_num <= PIC_IRQ_MAX){
		//slave PIC interrupt line
		outb((EOI|(irq_num&0x07)), SLAVE_8259_PORT);
		outb((EOI|(ICW3_SLAVE)), MASTER_8259_PORT);
	}
	else{
		printf("INVALID PIC IRQ NUMBER #%x: CANNOT SEND EOI", irq_num);
	}
	//restore flags and leave critical section
	restore_flags(flags);
	return;
}

