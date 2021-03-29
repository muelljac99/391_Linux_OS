
#include "multiboot.h"
#include "x86_desc.h"
#include "lib.h"
#include "i8259.h"
#include "irq_asm.h"
#include "service_irq.h"
#include "sys_call.h"
#include "terminal.h"

/* terminal driver buffer and flags */
volatile unsigned char key_buf[BUF_SIZE_MAX] = "";
volatile int key_buf_size = 0;
volatile int terminal_flag = 0;

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
		puts("    ");
		buf_fill(' ');
		buf_fill(' ');
		buf_fill(' ');
		buf_fill(' ');
	}
	else if(scan == ENTER){
		// enter will set the terminal flag indicating a read is done
		putc('\n');
		buf_fill('\n');
		terminal_flag = 0;
	}
	else if(scan == BACKSPACE){
		//don't backspace if keyboard buffer is empty
		if(key_buf_size == 0){
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
		putc(' ');
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
		if(key_buf_size != 0){
			key_buf_size--;
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
		// different character depending on caps lock and shift combo
		else if(caps==-1){
			if((rshift==1)||(lshift==1)){
				putc(both_code[scan]);
				buf_fill(both_code[scan]);
			}
			else{
				putc(cap_code[scan]);
				buf_fill(cap_code[scan]);
			}
		}
		else{
			if((rshift==1)||(lshift==1)){
				putc(shift_code[scan]);
				buf_fill(shift_code[scan]);
			}
			else{
				putc(scancode[scan]);
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
	return 0;
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
	
	//clear the keyboard buffer
	key_buf_size = 0;
	
	//wait until the user presses enter to get the buffer
	terminal_flag = 1;
	while(terminal_flag != 0);
	
	//get the smaller of the keyboard buffer size and requested bytes
	if(nbytes < key_buf_size){
		size = nbytes;
	}
	else{
		size = key_buf_size;
	}
	
	//copy the keyboard buffer into the terminal buffer
	for(i=0; i<size; i++){
		//make last character in the array newline
		if(i == size - 1){
			read_buf[i] = '\n';
		}
		else{
			read_buf[i] = key_buf[i];
		}
	}
	
	//clear the keyboard buffer
	key_buf_size = 0;
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
	if(key_buf_size < BUF_SIZE_MAX){
		key_buf[key_buf_size] = add;
		key_buf_size++;
	}
}
