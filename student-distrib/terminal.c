#include "multiboot.h"
#include "x86_desc.h"
#include "lib.h"
#include "i8259.h"
#include "irq_asm.h"
#include "service_irq.h"
#include "sys_call.h"
#include "sys_call_asm.h"
#include "terminal.h"
#include "paging.h"
#include "pit.h"

/* keyboard global flags */
int rshift = 0;
int lshift = 0;
int ctrl = 0;
int alt = 0;
int caps = 0;

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
		if(term_save[visible_term].key_buf_size < BUF_SIZE_MAX - 4){		// puts four characters into buffer
			putc_vis(' ');
			buf_fill(' ');
			putc_vis(' ');
			buf_fill(' ');
			putc_vis(' ');
			buf_fill(' ');
			putc_vis(' ');
			buf_fill(' ');
		}
	}
	else if(scan == ENTER){
		// enter will set the terminal flag indicating a read is done
		putc_vis('\n');
		buf_fill('\n');
		term_save[visible_term].terminal_flag = 0;
	}
	else if(scan == BACKSPACE){
		//don't backspace if keyboard buffer is empty
		if(term_save[visible_term].key_buf_size == 0){
			return;
		}
		// move the screen location back one
		if(get_x() == 0){
			if(get_y() != 0){
				set_x(NUM_COLS-1);
				set_y(get_y()-1);
			}
		}
		else{
			set_x(get_x() - 1);
		}
		// clear the character there
		putc_vis(' ');
		if(get_x() == 0){
			set_x(NUM_COLS-1);
			set_y(get_y()-1);
		}
		else{
			set_x(get_x() - 1);
		}
		// move cursor back one
		update_cursor(get_x(), get_y());
		// remove element from the terminal buffer
		if(term_save[visible_term].key_buf_size != 0){
			term_save[visible_term].key_buf_size--;
		}
		//get and set functions for the video memory locations (screen_x and screen_y)
		//move back a space and putc_vis a ' '
	}
	else if(scan == F1 && alt == 1){
		if(visible_term != 0){
			switch_terminals(0);
		}
	}
	else if(scan == F2 && alt == 1){
		if(visible_term != 1){
			switch_terminals(1);
		}
	}
	else if(scan == F3 && alt == 1){
		if(visible_term != 2){
			switch_terminals(2);
		}
	}
	else if(scan < 0x40 && scancode[scan] != 0){
		if((ctrl == 1)&&(scancode[scan] == 'l')){
			clear();
		}
		// different character depending on caps lock and shift combo
		else if(term_save[visible_term].key_buf_size >= BUF_SIZE_MAX - 1){
			return;
		}
		else if(caps==-1){
			if((rshift==1)||(lshift==1)){
				putc_vis(both_code[scan]);
				buf_fill(both_code[scan]);
			}
			else{
				putc_vis(cap_code[scan]);
				buf_fill(cap_code[scan]);
			}
		}
		else{
			if((rshift==1)||(lshift==1)){
				putc_vis(shift_code[scan]);
				buf_fill(shift_code[scan]);
			}
			else{
				putc_vis(scancode[scan]);
				buf_fill(scancode[scan]);
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
	
	//initialize the keyboard buffers and flags for each terminal
	int i;
	for(i=0; i<NUM_TERMINAL; i++){
		strcpy((int8_t*)term_save[i].key_buf, "");
		term_save[i].key_buf_size = 0;
		term_save[i].terminal_flag = 0;
	}
}

/* 
 * terminal_open
 *   DESCRIPTION: Terminal driver open function that does nothing
 *   INPUTS: filename -- unused
 *   OUTPUTS: none
 *   RETURN VALUE: 0 on success, -1 on fail
 *   SIDE EFFECTS: none
 */
int32_t terminal_open(const uint8_t* filename){
	// does nothing
	return 0;
}

/* 
 * terminal_close
 *   DESCRIPTION: Terminal driver close function that does nothing
 *   INPUTS: fd -- unused
 *   OUTPUTS: none
 *   RETURN VALUE: 0 on success, -1 on fail
 *   SIDE EFFECTS: none
 */
int32_t terminal_close(int32_t fd){
	// does nothing
	return -1;
}

/* 
 * terminal_read
 *   DESCRIPTION: Terminal driver read function that takes input from the keyboard buffer and writes into
 * 				  the terminal buffer upon an enter press.
 *   INPUTS: fd -- unused
 *			 nbytes -- the number of bytes to read from the keyboard buffer
 *   OUTPUTS: buf -- the buffer that the function writes into
 *   RETURN VALUE: number of bytes read
 *   SIDE EFFECTS: clears the keyboard buffer
 */
int32_t terminal_read(int32_t fd, void* buf, int32_t nbytes){
	unsigned char* read_buf = (unsigned char*)buf;
	int i, size;
	
	//check for valid parameters
	if((read_buf==NULL)||(nbytes < 0)){
		return -1;
	}
	
	//clear the keyboard buffer					// might need to change this to be active instead of visibile
	term_save[active_term].key_buf_size = 0;
	
	//wait until the user presses enter to get the buffer
	term_save[active_term].terminal_flag = 1;
	while(term_save[active_term].terminal_flag != 0);
	
	//get the smaller of the keyboard buffer size and requested bytes
	if(nbytes < term_save[active_term].key_buf_size){
		size = nbytes;
	}
	else{
		size = term_save[active_term].key_buf_size;
	}
	
	//copy the keyboard buffer into the terminal buffer
	for(i=0; i<size; i++){
		//make last character in the array newline
		if(i == size - 1){
			read_buf[i] = '\n';
		}
		else{
			read_buf[i] = term_save[active_term].key_buf[i];
		}
	}
	
	//clear the keyboard buffer
	term_save[active_term].key_buf_size = 0;
	return i;
}

/* 
 * terminal_write
 *   DESCRIPTION: Terminal driver write function that writes the given buffer to the screen
 *   INPUTS: fd -- unused
 * 			 buf -- the input buffer containing the characters to be written to the screen
 * 			 nbytes -- the number of bytes to write to the screen
 *   OUTPUTS: none
 *   RETURN VALUE: number of bytes written to the screen
 *   SIDE EFFECTS: none
 */
int32_t terminal_write(int32_t fd, const void* buf, int32_t nbytes){
	unsigned char* write_buf = (unsigned char*)buf;
	int i;
	
	//check for valid parameters
	if((write_buf==NULL)||(nbytes < 0)){
		return -1;
	}
	
	//put specified number of characters onto the screen
	for(i=0; i<nbytes; i++){
		putc(write_buf[i]);
	}
	return i;
}

/*
 * init_terminals
 */
void init_terminals(void){
	// clear the screen and initialize the video memory save for each terminal
	clear();
	memcpy((void*)TERM1_VID, (void*)VIDEO_START, VID_SIZE);
	visible_term = 1;
	clear();
	memcpy((void*)TERM2_VID, (void*)VIDEO_START, VID_SIZE);
	visible_term = 2;
	clear();
	memcpy((void*)TERM3_VID, (void*)VIDEO_START, VID_SIZE);
	visible_term = 0;
	clear();
	
	//initialize the remaining values in the terminal info structures
	int i;
	for(i=0; i<NUM_TERMINAL; i++){
		term_save[i].cursor_x = 0;
		term_save[i].cursor_y = 0;
		term_save[i].user_process_num = 0;
		term_save[i].init = 0;
	}
	
	//initialize the PIT for scheduling to begin
	init_pit();
	
	/* Execute the first program ("shell") for the first terminal */
	term_save[0].init = 1;
	__sys_execute((uint8_t*)"shell", 1);			//process 0 -> base of terminal 1
}

/*
 * switch_terminals
 */
int32_t switch_terminals(uint32_t term_num){
	//set the printing synch flag
	switch_terms_flag = term_num;
	
	if(putc_flag == 1){
		//return without disabling the switch_terms_flag
		return 0;
	}
	
	//check that the switch actually needs to be performed
	if(term_num >= NUM_TERMINAL || term_num == visible_term){
		return -1;
		printf("INVALID TERMINAL NUMBER\n");
	}
	//save the current video screen to the save spot
	switch(visible_term){
		case 0 : 
			memcpy((void*)TERM1_VID, (void*)VIDEO_START, VID_SIZE);
			break;
		case 1 : 
			memcpy((void*)TERM2_VID, (void*)VIDEO_START, VID_SIZE);
			break;
		case 2 : 
			memcpy((void*)TERM3_VID, (void*)VIDEO_START, VID_SIZE);
			break;
		default :
			break;
	}
	
	//save the curr terminal cursor position
	term_save[visible_term].cursor_x = get_x();
	term_save[visible_term].cursor_y = get_y();
	
	//load the new terminal's saved video memory
	switch(term_num){
		case 0:
			memcpy((void*)VIDEO_START, (void*)TERM1_VID, VID_SIZE);
			break;
		case 1:
			memcpy((void*)VIDEO_START, (void*)TERM2_VID, VID_SIZE);
			break;
		case 2:
			memcpy((void*)VIDEO_START, (void*)TERM3_VID, VID_SIZE);
			break;
		default :
			break;
	}
	
	//update the cursor position and character position
	update_cursor(term_save[term_num].cursor_x, term_save[term_num].cursor_y);
	set_x(term_save[term_num].cursor_x);
	set_y(term_save[term_num].cursor_y);
	
	//find the process number we want to switch to
	int process_num = term_save[term_num].user_process_num;
	pcb_t* curr_pcb = (pcb_t*)(KERNEL_BASE-((process_num+1)*KERNEL_STACK));
	
	//change the vidmap page
	if(curr_pcb->vidmap_flag == 1){
		//change the vidmap page to map to the true video memory
		user_table[USER_VID_PAGE_IDX].page_addr = (VIDEO_START >> FOUR_KB_SHIFT);
	}
	
	visible_term = term_num;
	
	/* active process switch moved to the scheduler
	
	// switch the currently executing or active process to the one specified by the terminal
	if(term_save[term_num].init == 0){
		//save the esp value
		term_save[curr_term].esp_save = get_esp();
		term_save[curr_term].ebp_save = get_ebp();
		
		// if the terminal hasn't been initialized yet then execute the base shell
		term_save[term_num].init = 1;
		curr_term = term_num;
		
		//enable interrupts in PIC for the keyboard for the new terminal
		enable_irq(KEYBOARD_IRQ);
		
		__sys_execute((uint8_t*)"shell", 1);
	}
	else{
		switch_active(term_num);
	}
	*/
	
		
	//disable the print synch flag
	switch_terms_flag = -1;
	
	return 0;
}

/*
 * switch_active
 */
int32_t switch_active(uint32_t term_num){
	//check valid terminal number
	if(term_num >= NUM_TERMINAL){
		return -1;
		printf("INVALID TERMINAL NUMBER\n");
	}
	
	//find the process number we want to switch to
	int process_num = term_save[term_num].user_process_num;
	pcb_t* curr_pcb = (pcb_t*)(KERNEL_BASE-((process_num+1)*KERNEL_STACK));
	
	//check for valid process_num
	if(process_num >= MAX_PROCESS){
		return -1;
		printf("INVALID PROCESS NUMBER\n");
	}
	if(process_present[process_num] != 1){
		return -1;
		printf("INVALID PROCESS\n");
	}
	
	// set up the user process page to map to the correct physical user process image
	page_dir[USER_PAGE_IDX].table_addr = ((USER_START + (process_num * PAGE_SIZE)) >> FOUR_KB_SHIFT);
	// clear the TLB
	tlb_flush();
	
	//change the vidmap page
	if(curr_pcb->vidmap_flag == 1){
		if(term_num == visible_term){
			//change the vidmap page to map to the true video memory
			user_table[USER_VID_PAGE_IDX].page_addr = (VIDEO_START >> FOUR_KB_SHIFT);
		}
		else{
			//change the vidmap page to map to the saved video memory of term_num
			if(term_num == 0)
				user_table[USER_VID_PAGE_IDX].page_addr = (TERM1_VID >> FOUR_KB_SHIFT);
			else if(term_num == 1)
				user_table[USER_VID_PAGE_IDX].page_addr = (TERM2_VID >> FOUR_KB_SHIFT);
			else
				user_table[USER_VID_PAGE_IDX].page_addr = (TERM3_VID >> FOUR_KB_SHIFT);
		}
	}
	
	//save the curent esp pointer before moving to the new stack
	term_save[active_term].esp_save = get_esp();
	term_save[active_term].ebp_save = get_ebp();
	
	//update the terminal number
	active_term = term_num;
	
	//update the tss to match the new process
	tss.esp0 = KERNEL_BASE-((process_num)*KERNEL_STACK)-4;
	tss.ss0 = KERNEL_DS;
	
	//assembly linkage function to switch stacks
	switch_stack(term_save[active_term].esp_save, term_save[active_term].ebp_save);
	
	return 0;
}
	
/* 
 * buf_fill
 *   DESCRIPTION: Helper function used to add a character to the keyboard buffer and keep track
 * 				  of the size of the keyboard buffer
 *   INPUTS: add -- the character to be added to the keyboard buffer
 *   OUTPUTS: none
 *   RETURN VALUE: none
 *   SIDE EFFECTS: may add values to the keyboard buffer
 */
void buf_fill(unsigned char add){
	// need to save space for a newline at the end of the buffer
	if(term_save[visible_term].key_buf_size < BUF_SIZE_MAX){
		term_save[visible_term].key_buf[term_save[visible_term].key_buf_size] = add;
		term_save[visible_term].key_buf_size++;
	}
}
