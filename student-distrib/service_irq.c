
#include "multiboot.h"
#include "x86_desc.h"
#include "lib.h"
#include "i8259.h"
#include "irq_asm.h"
#include "service_irq.h"
#include "sys_call.h"
#include "rtc.h"

const char exception_resp[NUM_EXCEPTION][32] = {"Division by Zero", "Single-step Interrupt", "Non-Maskablee Interrupt",
												"Breakpoint", "Overflow", "Bounds", "Invalid Opcode", "Coprocessor N/A",
												"Double Fault", "Coprocessor Seg Over", "Invalid Task State Seg", "Segment Not Present",
												"Stack Fault", "General Protection Fault", "Page Fault", "reserved",
												"Math Fault", "Alignment Check", "Machine Check", "SIMD Float Exception",
												"Virtualization Exception", "Control Protection Exception", "Intel reserved",
												"Intel reserved", "Intel reserved", "Intel reserved", "Intel reserved",
												"Intel reserved", "Intel reserved", "Intel reserved", "Intel reserved",
												"Intel reserved"};
												
/* keyboard global flags */
int rshift = 0;
int lshift = 0;
int ctrl = 0;
int alt = 0;
int caps = 0;

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
		clear();
		printf("EXCEPTION #%x: %s\n", irq, exception_resp[irq]);
		while(1);
	}
	else if(irq == SYS_CALL_IRQ){
		do_sys_call(reg->EAX, reg->EBX, reg->ECX, reg->EDX);
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

/* 
 * handle_keyboard
 *   DESCRIPTION: The handler function for the keyboard. This takes in the scancode from the keyboard port and determines
 * 				  the associated character to print.
 *   INPUTS: none
 *   OUTPUTS: none
 *   RETURN VALUE: none
 *   SIDE EFFECTS: places a character onto the screen
 */
void handle_keyboard(void){
	char scancode[] = {0 , 0, '1', '2', '3','4', '5', '6', '7', '8', '9', '0', '-', '=', 0, 0, 'q', 'w', 'e', 'r', 't', 'y', 'u', 'i',
					   'o', 'p', '[', ']', 0, 0, 'a', 's', 'd', 'f', 'g', 'h', 'j', 'k', 'l', ';', '\'', '`', 0, '\\', 'z', 'x', 'c', 
					   'v', 'b', 'n', 'm', ',', '.', '/', 0, 0, 0, ' '};
	char cap_code[] = {0 , 0, '1', '2', '3','4', '5', '6', '7', '8', '9', '0', '-', '=', 0, 0, 'Q', 'W', 'E', 'R', 'T', 'Y', 'U', 'I',
					   'O', 'P', '[', ']', 0, 0, 'A', 'S', 'D', 'F', 'G', 'H', 'J', 'K', 'L', ';', '\'', '`', 0, '\\', 'Z', 'X', 'C', 
					   'V', 'B', 'N', 'M', ',', '.', '/', 0, 0, 0, ' '};
	char both_code[] = {0 , 0, '!', '@', '#','$', '%', '^', '&', '*', '(', ')', '_', '+', 0, 0, 'q', 'w', 'e', 'r', 't', 'y', 'u', 'i',
					   'o', 'p', '{', '}', 0, 0, 'a', 's', 'd', 'f', 'g', 'h', 'j', 'k', 'l', ':', '\"', '~', 0, '|', 'z', 'x', 'c',
					   'v', 'b', 'n', 'm', '<', '>', '?', 0, 0, 0, ' '};
	char shift_code[] = {0 , 0, '!', '@', '#','$', '%', '^', '&', '*', '(', ')', '_', '+', 0, 0, 'Q', 'W', 'E', 'R', 'T', 'Y', 'U', 'I',
					   'O', 'P', '{', '}', 0, 0, 'A', 'S', 'D', 'F', 'G', 'H', 'J', 'K', 'L', ':', '\"', '~', 0, '|', 'Z', 'X', 'C', 
					   'V', 'B', 'N', 'M', '<', '>', '?', 0, 0, 0, ' '};
	uint32_t scan;
	scan = inb(KEYBOARD_PORT);
	// 0x40 is the number of elements that we have characters chosen for
	if(scan == LSHIFT){
		lshift = 1;
	}
	else if(scan == LSHIFT_R){
		lshift = 0;
	}
	else if(scan == RSHIFT){
		rshift = 1;
	}
	else if(scan == RSHIFT_R){
		rshift = 0;
	}
	else if(scan == CTRL){
		ctrl = 1;
	}
	else if(scan == CTRL_R){
		ctrl = 0;
	}
	else if(scan == ALT){
		alt = 1;
	}
	else if(scan == ALT_R){
		alt = 0;
	}
	else if(scan == CAPS_LOCK){
		caps = ~caps;
	}
	else if(scan == TAB){
		puts("    ");
	}
	else if(scan == ENTER){
		putc('\n');
	}
	else if(scan == BACKSPACE){
		if(get_x() == 0){
			if(get_y() != 0){
				set_x(NUM_COLS-1);
				set_y(get_y()-1);
			}
		}
		else{
			set_x(get_x() - 1);
		}
		putc(' ');
		if(get_x() == 0){
			set_x(NUM_COLS-1);
			set_y(get_y()-1);
		}
		else{
			set_x(get_x() - 1);
		}
		//get and set functions for the video memory locations (screen_x and screen_y)
		//move back a space and putc a ' '
	}
	else if(scan == F1){
		// do nothing
	}
	else if(scan < 0x40 && scancode[scan] != 0){
		if((ctrl == 1)&&(scancode[scan] == 'l')){
			clear();
		}
		else if(caps==-1){
			if((rshift==1)||(lshift==1)){
				putc(both_code[scan]);
			}
			else{
				putc(cap_code[scan]);
			}
		}
		else{
			if((rshift==1)||(lshift==1)){
				putc(shift_code[scan]);
			}
			else{
				putc(scancode[scan]);
			}
		}
	}
	return;
}

/* 
 * init_keyboard
 *   DESCRIPTION: Initialize the keyboard by setting the idt entry accordingly and unmasking the irq line.
 *   INPUTS: none
 *   OUTPUTS: none
 *   RETURN VALUE: none
 *   SIDE EFFECTS: changes the idt
 */
void init_keyboard(void){
	idt[KEYBOARD_IRQ].present = 1;
	irq_handlers[KEYBOARD_IRQ - BASE_INT] = handle_keyboard;
	enable_irq(KEYBOARD_IRQ);
}
