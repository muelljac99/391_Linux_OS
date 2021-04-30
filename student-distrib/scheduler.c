
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
#include "terminal.h"

void scheduler(void){
	uint32_t term_num;
	
	//choose the next active terminal
	term_num = (active_term + 1)%NUM_TERMINAL;
	
	// switch the currently executing or active process to the one specified by the terminal
	if(term_save[term_num].init == 0){
		//save the esp value
		term_save[active_term].esp_save = get_esp();
		term_save[active_term].ebp_save = get_ebp();
		
		// if the terminal hasn't been initialized yet then execute the base shell
		term_save[term_num].init = 1;
		active_term = term_num;
		
		//enable interrupts in PIC for the keyboard for the new terminal
		enable_irq(KEYBOARD_IRQ);
		
		__sys_execute((uint8_t*)"shell", 1);
	}
	else{
		switch_active(term_num);
	}
}
