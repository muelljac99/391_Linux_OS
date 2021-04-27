
#include "types.h"
#include "x86_desc.h"
#include "sys_call_asm.h"
#include "sys_call.h"
#include "lib.h"
#include "rtc.h"
#include "terminal.h"
#include "file_dir.h"
#include "paging.h"

uint32_t num_vidmaps = 0;

/* 
 * sys_halt
 *   DESCRIPTION: This is the system call wrapper function for system halt. It is used to allow
 * 				  for a 8-bit status argument from the system
 *   INPUTS: status -- information about how the user process terminated
 *   OUTPUTS: none
 *   RETURN VALUE: returns the status value to the parent process
 *   SIDE EFFECTS: changes current process to the parent
 */
int32_t sys_halt(uint8_t status) {
	return __sys_halt((uint32_t)status);
}

/* 
 * __sys_halt
 *   DESCRIPTION: This is the base system halt function. This function allows for larger than 32-bit
 *				  inputs for exception handling. This function is called when one user process
 * 				  ends and the parent process should resume
 *   INPUTS: status -- information about how the user process terminated
 *   OUTPUTS: none
 *   RETURN VALUE: returns the status value to the parent process
 *   SIDE EFFECTS: changes current process to the parent
 */
int32_t __sys_halt(uint32_t status){
	uint32_t process_num = 0;
	int i;
	
	//get the current pcb pointer
	pcb_t* curr_pcb = (pcb_t*)(get_esp()&PCB_MASK);
	
	if (curr_pcb->parent_process_num == ORPHAN) {
		// if attempting to close the base shell, start a new one at the same process number
		process_present[term_save[curr_term].user_process_num] = 0;
		__sys_execute((uint8_t*)SHELL_NAME, 1);
	}
	
	//if this user process used a vidmap then destroy it
	if(curr_pcb->vidmap_flag == 1){
		if(num_vidmaps == 1){
			if(user_table[USER_VID_PAGE_IDX].present == 1){
				user_table[USER_VID_PAGE_IDX].present = 0;
				user_table[USER_VID_PAGE_IDX].page_addr = NULL;
			}
			if(page_dir[USER_VID_DIR_IDX].present == 1){
				page_dir[USER_VID_DIR_IDX].present = 0;
				page_dir[USER_VID_DIR_IDX].table_addr = NULL;
			}
		}
		curr_pcb->vidmap_flag = 0;
		num_vidmaps--;
	}
	
	//restore the parent tss info
	//the tss esp0 must point to start not current otherwise stack will build up
	//subtract 4 from the base to remain in the page
	tss.esp0 = KERNEL_BASE-((curr_pcb->parent_process_num)*KERNEL_STACK)-4;
	tss.ss0 = curr_pcb->parent_ss0;
	
	//restore the parent paging info
	page_dir[USER_PAGE_IDX].table_addr = ((USER_START + (curr_pcb->parent_process_num * PAGE_SIZE)) >> FOUR_KB_SHIFT);
	// clear the TLB
	tlb_flush();
	
	//close all the open files and devices
	for(i=0; i<MAX_FILE; i++){
		if(curr_pcb->file_array[i].present == 1){
			sys_close(i);
		}
	}
	
	//clear the executable and argument buffers
	strncpy((int8_t*)curr_pcb->exe_name, (int8_t*)"", NAME_LEN);
	strncpy((int8_t*)curr_pcb->arg_buf, (int8_t*)"", ARG_BUF_SIZE);
	
	//set process to available and change process number
	process_present[term_save[curr_term].user_process_num] = 0;
	process_num = curr_pcb->parent_process_num;
	
	//set the terminal process number to the parent
	term_save[curr_term].user_process_num = process_num;
	
	//jump to the execute return
	halt_jump((uint32_t)status, (uint32_t)curr_pcb->parent_esp0);
	
	// if we get here then something went wrong
	return (int32_t)status;
}

/* 
 * sys_execute
 *   DESCRIPTION: This is the execute system call. It is a wrapper for the kernel specific system execute function
 *   INPUTS: command -- a string containing the executable name and any arguments for that executable
 *   OUTPUTS: none
 *   RETURN VALUE: the status of the new process's termination (received from halt)
 *   SIDE EFFECTS: changes current process to the child
 */
int32_t sys_execute(const uint8_t* command){
	return __sys_execute(command, 0);
}

/* 
 * __sys_execute
 *   DESCRIPTION: This is the kernel specific execute system call. It creates a new process for a specified
 *				  executable function and gives the new process control of the system.
 *   INPUTS: command -- a string containing the executable name and any arguments for that executable
 *			 orphan -- 1 if the process should be an orphan or is a base process
 *   OUTPUTS: none
 *   RETURN VALUE: the status of the new process's termination (received from halt)
 *   SIDE EFFECTS: changes current process to the child
 */
int32_t __sys_execute(const uint8_t* command, uint32_t orphan){
	/* string editing variables */
	uint32_t command_len = strlen((int8_t*) command);
	dentry_t exe_dentry;
	uint32_t exe_size;
	uint32_t word_flag = 0;
	uint32_t word_break;
	uint8_t exe[ARG_BUF_SIZE] = "";
	uint8_t args[ARG_BUF_SIZE] = "";
	int i;
	
	/* process initialization variables */
	uint32_t parent_num;
	uint32_t process_num = 0;
	pcb_t* child_pcb;
	uint8_t start_inst[4];		//starting address is 4bytes long
	uint32_t start_addr = 0;
	uint32_t status;
	
	/* available process counters */
	uint32_t uninit_terms = 0;
	uint32_t num_processes = 0;
	
	//split up by spaces to get executable word and arguments
	for(i=0; i<command_len; i++){
		if((command[i] == ' ')&&(word_flag == 0)){
			word_break = i+1;
			word_flag = 1;
		}
		else if(word_flag == 0){
			exe[i] = command[i];
		}
		else{
			args[i-word_break] = command[i];
		}
	}
	args[i] = NULL;
	
	//executable check (look for the ELF magic)
	if(exe_check(exe) != 0){
		return -1;
	}
	
	//leave space for the uninitialized terminals
	for(i=0; i<NUM_TERMINAL; i++){
		if(term_save[i].init == 0){
			uninit_terms++;
		}
	}
	for(i=0; i<MAX_PROCESS; i++){
		num_processes += process_present[i];
	}
	if(num_processes >= MAX_PROCESS - uninit_terms){
		printf("CANNOT EXECUTE ADDITIONAL PROCESS ON THIS TERMINAL\n");
		return -1;
	}
	
	//get the new process number for the child
	if (orphan == 1) {
		parent_num = ORPHAN;
	}
	else{
		parent_num = term_save[curr_term].user_process_num;
	}
	for(i=0; i<MAX_PROCESS; i++){
		if(process_present[i] == 0){
			// this is an open process location
			process_num = i;
			break;
		}
	}
	if(i == MAX_PROCESS){
		// no available process spots
		printf("NO AVAILABLE PROCESS SPOTS\n");
		return -1;
	}
	process_present[i] = 1;
	
	//set the terminal process number to the child
	term_save[curr_term].user_process_num = process_num;
	
	//set up the paging for the new process
	page_dir[USER_PAGE_IDX].table_addr = ((USER_START + (process_num * PAGE_SIZE)) >> FOUR_KB_SHIFT);
	// clear the TLB
	tlb_flush();
	
	//load the program to the program image space in the page at virtual addr 128MB
	read_dentry_by_name(exe, &exe_dentry);
	exe_size = (*(file_sys_addr + ((1+exe_dentry.inode_num)*(INODE_SIZE/4))));		//divide by 4 to get # of uint's
	read_data(exe_dentry.inode_num, 0, (uint8_t*)USER_PROG_IMG, exe_size);
	
	//get the starting instruction
	read_data(exe_dentry.inode_num, EXE_EIP_OFFSET, start_inst, 4); 		//reads the 4-byte address
	for(i=3; i>=0; i--){
		//combine the four 8-bit values into one 32-bit
		start_addr = start_addr << 8;
		start_addr += (uint32_t)start_inst[i];
	}
	
	//get the pcb ptr for the child process
	child_pcb = (pcb_t*)(KERNEL_BASE-((process_num+1)*KERNEL_STACK));
	
	//fill the process exe name and arguments buffer
	strncpy((int8_t*)child_pcb->exe_name, (int8_t*)exe, NAME_LEN);
	strncpy((int8_t*)child_pcb->arg_buf, (int8_t*)args, ARG_BUF_SIZE);
	
	//fill the parent process info of the child pcb
	child_pcb->parent_process_num = parent_num;
	child_pcb->parent_pcb_ptr = (pcb_t*)(get_esp()&PCB_MASK);
	child_pcb->parent_esp0 = get_esp();
	child_pcb->parent_ss0 = tss.ss0;
	
	//set the vidmap flag and rtc frequency
	child_pcb->vidmap_flag = 0;
	child_pcb->rtc_freq = 0;
	
	//set up the file array
	for(i=0; i<MAX_FILE; i++){
		// initialized so file array is empty
		child_pcb->file_array[i].present = 0;
	}
	
	//set the standard in and out in the file array to the terminal driver (entries 0 and 1)
	for(i=0; i<2; i++){
		child_pcb->file_array[i].dev_open = terminal_open;
		child_pcb->file_array[i].dev_close = terminal_close;
		child_pcb->file_array[i].dev_read = terminal_read;
		child_pcb->file_array[i].dev_write = terminal_write;
		child_pcb->file_array[i].inode = 0;				//unused
		child_pcb->file_array[i].file_pos = 0; 			//unused
		child_pcb->file_array[i].present = 1;
	}
	
	//set up the tss with the new process info
	//subtract 4 from the base to remain in the page
	tss.esp0 = KERNEL_BASE-((process_num)*KERNEL_STACK)-4;
	tss.ss0 = KERNEL_DS;
	
	//context switch and IRET (after halt we come back here)
	status = push_context(start_addr);
	
	return (int32_t)status;
}

/* 
 * sys_read
 *   DESCRIPTION: This is the common C function for all read system calls. This function performs a
 * 				  specific read function if a valid file array entry is specified
 *   INPUTS: fd -- the file descriptor or index to the file array of the device to be read
 * 			 buf -- a buffer of unknown type that may be filled by the read function
 * 			 nbytes -- the number of bytes to be read and filled into the buf
 *   OUTPUTS: none
 *   RETURN VALUE: return value of specific read function on success, -1 on failure
 *   SIDE EFFECTS: may overwrite the contents of buf
 */
int32_t sys_read(int32_t fd, void* buf, int32_t nbytes){
	pcb_t* curr_pcb = (pcb_t*)(get_esp()&PCB_MASK);
	// check that the file descriptor is valid (can't read from stdout)
	if(fd < 0 || fd >= MAX_FILE || fd == 1){
		return -1;
	}
	else if(curr_pcb->file_array[fd].present == 0){
		return -1;
	}
	else{
		// if it is valid then run the corresponding read function
		return (*curr_pcb->file_array[fd].dev_read)(fd, buf, nbytes);
	}
}

/* 
 * sys_write
 *   DESCRIPTION: This is the common C function for all write system calls. This function performs a
 * 				  specific write function if a valid file array entry is specified
 *   INPUTS: fd -- the file descriptor or index to the file array of the device to be written
 * 			 buf -- a buffer of unknown type that may be read by the write function
 * 			 nbytes -- the number of bytes to be written from the buf
 *   OUTPUTS: none
 *   RETURN VALUE: 0 on success, -1 on failure
 *   SIDE EFFECTS: none
 */
int32_t sys_write(int32_t fd, const void* buf, int32_t nbytes){
	pcb_t* curr_pcb = (pcb_t*)(get_esp()&PCB_MASK);
	// check that the file descriptor is valid (can't write to stdin)
	if(fd < 0 || fd >= MAX_FILE || fd == 0){
		return -1;
	}
	else if(curr_pcb->file_array[fd].present == 0){
		return -1;
	}
	else{
		// if it is valid then run the corresponding write function
		return (*curr_pcb->file_array[fd].dev_write)(fd, buf, nbytes);
	}
}

/* 
 * sys_open
 *   DESCRIPTION: This is the common C function for all open system calls. It fills the first available
 * 				  file array entry with the info for a given file, device, or directory then performs
 * 				  specific open function.
 *   INPUTS: filename -- a string holding the name of the file, device, or directory to be opened
 *   OUTPUTS: none
 *   RETURN VALUE: 0 on success, -1 on failure
 *   SIDE EFFECTS: may edit file array
 */
int32_t sys_open(const uint8_t* filename){
	int fd;
	pcb_t* curr_pcb = (pcb_t*)(get_esp()&PCB_MASK);
	dentry_t dentry;
	unsigned char rtc[RTC_NAME_LEN] = RTC_NAME;
	unsigned char term[TERMINAL_NAME_LEN] = TERMINAL_NAME;
	
	//find a spot in the file array
	for(fd = 0; fd < MAX_FILE; fd++){
		if(curr_pcb->file_array[fd].present == 0){
			break;
		}
	}
	if(fd == MAX_FILE){
		printf("NO AVAILABLE FILE ARRAY ENTRY\n");
		return -1;
	}
	
	// set up the file array entry with the info for the device/file
	if(strncmp(filename, rtc, RTC_NAME_LEN) == 0){
		// set up the entry for the rtc
		curr_pcb->file_array[fd].dev_open = rtc_open;
		curr_pcb->file_array[fd].dev_close = rtc_close;
		curr_pcb->file_array[fd].dev_read = rtc_read;
		curr_pcb->file_array[fd].dev_write = rtc_write;
		curr_pcb->file_array[fd].inode = 0; 				//unused
		curr_pcb->file_array[fd].file_pos = 0;			//unused
		curr_pcb->file_array[fd].present = 1;				
		
		//run rtc open now
		if((*curr_pcb->file_array[fd].dev_open)(filename) == -1){
			return -1;
		}
	}
	
	else if(strncmp(filename, term, TERMINAL_NAME_LEN) == 0){
		// set up the entry for the terminal
		curr_pcb->file_array[fd].dev_open = terminal_open;
		curr_pcb->file_array[fd].dev_close = terminal_close;
		curr_pcb->file_array[fd].dev_read = terminal_read;
		curr_pcb->file_array[fd].dev_write = terminal_write;
		curr_pcb->file_array[fd].inode = 0;				//unused
		curr_pcb->file_array[fd].file_pos = 0; 			//unused
		curr_pcb->file_array[fd].present = 1;
		
		// run terminal open now
		if((*curr_pcb->file_array[fd].dev_open)(filename) == -1){
			return -1;
		}
	}
	
	else{
		if(read_dentry_by_name(filename, &dentry) == -1){
			//no matching file
			return -1;
		}
		if(dentry.file_type == TYPE_DIR){
			//this is a directory
			curr_pcb->file_array[fd].dev_open = dir_open;
			curr_pcb->file_array[fd].dev_close = dir_close;
			curr_pcb->file_array[fd].dev_read = dir_read;
			curr_pcb->file_array[fd].dev_write = dir_write;
			curr_pcb->file_array[fd].inode = 0;				//unused
			curr_pcb->file_array[fd].file_pos = 0; 			//becomes directory index
			curr_pcb->file_array[fd].present = 1;
			
			// run directory open now
			if((*curr_pcb->file_array[fd].dev_open)(filename) == -1){
				return -1;
			}
		}
		else if(dentry.file_type == TYPE_FILE){
			//this is a normal file
			curr_pcb->file_array[fd].dev_open = file_open;
			curr_pcb->file_array[fd].dev_close = file_close;
			curr_pcb->file_array[fd].dev_read = file_read;
			curr_pcb->file_array[fd].dev_write = file_write;
			curr_pcb->file_array[fd].inode = dentry.inode_num;
			curr_pcb->file_array[fd].file_pos = 0;			//starts at 0
			curr_pcb->file_array[fd].present = 1;
			
			// run file open now
			if((*curr_pcb->file_array[fd].dev_open)(filename) == -1){
				return -1;
			}
		}
		else{
			//invalid file type
			return -1;
		}
	}
	return fd;
}

/* 
 * sys_close
 *   DESCRIPTION: This is the common C function for all close system calls. This function performs
 * 				  the device specific close then empties the specified file array entry.
 *   INPUTS: fd -- the file descriptor or index to the file array of the device to be closed
 *   OUTPUTS: none
 *   RETURN VALUE: 0 on success, -1 on failure
 *   SIDE EFFECTS: may edit file array
 */
int32_t sys_close(int32_t fd){
	pcb_t* curr_pcb = (pcb_t*)(get_esp()&PCB_MASK);
	uint32_t ret_val;
	// check that the file descriptor is valid (can't close the stdin and stdout)
	if(fd < 2 || fd >= MAX_FILE){
		return -1;
	}
	else if(curr_pcb->file_array[fd].present == 0){
		return -1;
	}
	else{
		// if it is valid then run the specific close function
		ret_val = (*curr_pcb->file_array[fd].dev_close)(fd);
		
		// empty the entry in the array
		curr_pcb->file_array[fd].dev_open = NULL;
		curr_pcb->file_array[fd].dev_close = NULL;
		curr_pcb->file_array[fd].dev_read = NULL;
		curr_pcb->file_array[fd].dev_write = NULL;
		curr_pcb->file_array[fd].inode = 0;				//unused
		curr_pcb->file_array[fd].file_pos = 0; 			//unused
		curr_pcb->file_array[fd].present = 0;
		
		//return with the value from the specific function
		return ret_val;
	}
}

/* 
 * sys_getargs
 *   DESCRIPTION: This is the common C function for all getargs system calls. This function fills
 *				  a passed in buffer with the command line arguments provided when the user program was called
 *   INPUTS: buf -- the buffer to be filled with the arguments
 *			 nbytes -- the number of bytes to copy from the arguments into the buffer
 *   OUTPUTS: none
 *   RETURN VALUE: 0 on success, -1 on fail
 *   SIDE EFFECTS: overwrites the passed buffer
 */
int32_t sys_getargs(uint8_t* buf, int32_t nbytes){
	pcb_t* curr_pcb = (pcb_t*)(get_esp()&PCB_MASK);
	//check for null buffer
	if(buf == NULL){
		return -1;
	}
	//check if the args buffer is empty
	if(strlen((int8_t*)curr_pcb->arg_buf) == 0){
		return -1;
	}
	//check that the argument string length fits in the passed buffer
	if(strlen((int8_t*)curr_pcb->arg_buf) > nbytes){
		return -1;
	}
	//fill the passed buffer with the arguments
	strncpy((int8_t*)buf, (int8_t*)curr_pcb->arg_buf, nbytes);
	return 0;
}

/* 
 * sys_vidmap
 *   DESCRIPTION: This is the common system call used by a user space function to create a user accessible
 *				  map to the video memory.
 *   INPUTS: screen_start -- a pointer to the pointer to the user space video memory
 *   OUTPUTS: none
 *   RETURN VALUE: 0 on success, -1 on fail
 *   SIDE EFFECTS: changes the page directory
 */
int32_t sys_vidmap(uint8_t** screen_start){
	pcb_t* curr_pcb = (pcb_t*)(get_esp()&PCB_MASK);
	//check that the ptr to the ptr is not null
	if(screen_start < (uint8_t**)USER_VIRT_START || screen_start >= (uint8_t**)USER_VIRT_END){
		return -1;
	}
	
	if(num_vidmaps == 0){
		//set up the page table for the user space video memory
		page_dir[USER_VID_DIR_IDX].table_addr = (((unsigned int)user_table) >> FOUR_KB_SHIFT);
		page_dir[USER_VID_DIR_IDX].available = 0;			// we are not using these bits
		page_dir[USER_VID_DIR_IDX].ignored = 0;
		page_dir[USER_VID_DIR_IDX].page_size = 0;			// 0 because this corresponds to a page table
		page_dir[USER_VID_DIR_IDX].zero_pad = 0;
		page_dir[USER_VID_DIR_IDX].accessed = DISABLE;
		page_dir[USER_VID_DIR_IDX].cache_dis = DISABLE;
		page_dir[USER_VID_DIR_IDX].write_thru = DISABLE;
		page_dir[USER_VID_DIR_IDX].user_super = USER;
		page_dir[USER_VID_DIR_IDX].read_write = ENABLE;
		page_dir[USER_VID_DIR_IDX].present = ENABLE;
		
		//set up the page for the user space video memory (4kB page)
		user_table[USER_VID_PAGE_IDX].page_addr = (VIDEO_START >> FOUR_KB_SHIFT);
		user_table[USER_VID_PAGE_IDX].available = 0;		//we are not using these bits
		user_table[USER_VID_PAGE_IDX].global = ENABLE;
		user_table[USER_VID_PAGE_IDX].zero_pad = 0;
		user_table[USER_VID_PAGE_IDX].dirty = DISABLE;
		user_table[USER_VID_PAGE_IDX].accessed = DISABLE;
		user_table[USER_VID_PAGE_IDX].cache_dis = DISABLE;
		user_table[USER_VID_PAGE_IDX].write_thru = DISABLE;
		user_table[USER_VID_PAGE_IDX].user_super = USER;
		user_table[USER_VID_PAGE_IDX].read_write = ENABLE;
		user_table[USER_VID_PAGE_IDX].present = ENABLE;
	}
	
	//adjust argument to point to user space video memory
	(*screen_start) = (uint8_t*)USER_VID_START;
	
	//set the vidmap flag
	curr_pcb->vidmap_flag = 1;
	num_vidmaps++;
	return 0;
}

/* 
 * sys_sethandler
 *   DESCRIPTION: Not Yet Implemented
 *   INPUTS: none
 *   OUTPUTS: none
 *   RETURN VALUE: none
 *   SIDE EFFECTS: none
 */
int32_t sys_sethandler(int32_t signum, void* handler_address){
	return -1;
}

/* 
 * sys_sigreturn
 *   DESCRIPTION: Not Yet Implemented
 *   INPUTS: none
 *   OUTPUTS: none
 *   RETURN VALUE: none
 *   SIDE EFFECTS: none
 */
int32_t sys_sigreturn(void){
	return -1;
}

/* 
 * exe_check
 *   DESCRIPTION: The helper function used to check whether a given string is a file and an executable
 *   INPUTS: name_buf -- the buffer containing the filename to be checked
 *   OUTPUTS: none
 *   RETURN VALUE: 0 on success, -1 on failure
 *   SIDE EFFECTS: opens a file and clobbers file_dentry global
 */
int32_t exe_check(uint8_t* name_buf){
	uint8_t file_start[4];			// first byte is null then next 3 should be "ELF"
	dentry_t exe_dentry;
	int32_t resp;
	
	// check that file exists and has ELF at the start
	resp = read_dentry_by_name(name_buf, &exe_dentry);
	if(resp != 0)
		return -1;
	resp = read_data(exe_dentry.inode_num, 0, file_start, 4);		//checking for null byte then ELF so 4 bytes
	if(resp != 4)
		return -1;
	if(strncmp(file_start + 1, (uint8_t*)"ELF", 3) != 0) 		//ignore the null byte
		return -1;
	//return 0 on success
	return 0;
}
