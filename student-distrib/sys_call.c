
#include "types.h"
#include "x86_desc.h"
#include "sys_call_asm.h"
#include "sys_call.h"
#include "lib.h"
#include "rtc.h"
#include "terminal.h"
#include "file_dir.h"
#include "paging.h"

/* current process number */
uint32_t process_num = 0;

int32_t sys_halt(uint8_t status) {
	return __sys_halt((uint32_t)status);
}

int32_t __sys_halt(uint32_t status){
	int i;
	
	//dont halt if this is the base shell process
	/*if(process_num == 0){
		printf("CANNOT HALT SHELL");
		return (int32_t)status;
	}*/
	
	
	//get the current pcb pointer
	pcb_t* curr_pcb = (pcb_t*)(get_esp()&PCB_MASK);
	
	if (curr_pcb->parent_process_num == ORPHAN) {
		process_present[0] = 0;
		sys_execute((uint8_t*)"shell");
	}
	
	//restore the parent tss info
	tss.esp0 = curr_pcb->parent_esp0;
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
	
	//set process to available and change process number
	process_present[process_num] = 0;
	process_num = curr_pcb->parent_process_num;
	
	//jump to the execute return
	halt_jump((uint32_t)status, (uint32_t)curr_pcb->parent_esp0);
	
	// if we get here then something went wrong
	return (int32_t)status;
}

int32_t sys_execute(const uint8_t* command){
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
	pcb_t* child_pcb;
	uint8_t start_inst[4];
	uint32_t start_addr = 0;
	uint32_t status;
	
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
	
	//executable check (look for the ELF magic)
	if(exe_check(exe) != 0){
		return -1;
	}
	
	//get the new process number for the child
	parent_num = process_num;
	for(i=0; i<MAX_PROCESS; i++){
		if(process_present[i] == 0){
			// this is an open process location
			process_num = i;
			if (process_num == 0) {
				parent_num = ORPHAN;
			}
			break;
		}
	}
	if(i == MAX_PROCESS){
		// no available process spots
		printf("NO AVAILABLE PROCESS SPOTS\n");
		return -1;
	}
	process_present[i] = 1;
	
	//set up the paging for the new process
	page_dir[USER_PAGE_IDX].table_addr = ((USER_START + (process_num * PAGE_SIZE)) >> FOUR_KB_SHIFT);
	// clear the TLB
	tlb_flush();
	
	//load the program to the program image space in the page at virtual addr 128MB
	read_dentry_by_name(exe, &exe_dentry);
	exe_size = (*(file_sys_addr + ((1+exe_dentry.inode_num)*(INODE_SIZE/4))));
	read_data(exe_dentry.inode_num, 0, (uint8_t*)USER_PROG_IMG, exe_size);
	
	//get the starting instruction
	read_data(exe_dentry.inode_num, 24, start_inst, 4);
	for(i=3; i>=0; i--){
		start_addr = start_addr << 8;
		start_addr += (uint32_t)start_inst[i];
	}
	
	//fill in the child pcb items
	child_pcb = (pcb_t*)(KERNEL_BASE-((process_num+1)*KERNEL_STACK));
	child_pcb->parent_process_num = parent_num;
	child_pcb->parent_pcb_ptr = (pcb_t*)(get_esp()&PCB_MASK);
	child_pcb->parent_esp0 = get_esp();
	child_pcb->parent_ss0 = tss.ss0;
	
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
	tss.esp0 = KERNEL_BASE-((process_num)*KERNEL_STACK)-4;
	tss.ss0 = KERNEL_DS;
	
	//context switch and IRET (after halt we come back here)
	status = push_context(start_addr);
	
	return (int32_t)status;
}

/* 
 * __sys_read__
 *   DESCRIPTION: This is the common C function for all read system calls. This function performs a
 * 				  specific read function if a valid file array entry is specified
 *   INPUTS: fd -- the file descriptor or index to the file array of the device to be read
 * 			 buf -- a buffer of unknown type that may be filled by the read function
 * 			 nbytes -- the number of bytes to be read and filled into the buf
 *   OUTPUTS: none
 *   RETURN VALUE: 0 on success, -1 on failure
 *   SIDE EFFECTS: may overwrite the contents of buf
 */
int32_t sys_read(int32_t fd, void* buf, int32_t nbytes){
	pcb_t* curr_pcb = (pcb_t*)(get_esp()&PCB_MASK);
	// check that the file descriptor is valid
	if(fd < 0 || fd >= MAX_FILE){
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
 * __sys_write__
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
	// check that the file descriptor is valid
	if(fd < 0 || fd >= MAX_FILE){
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
 * __sys_open__
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
		printf("NO AVAILABLE FILE ARRAY ENTRY");
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
		(*curr_pcb->file_array[fd].dev_open)(filename);
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
		(*curr_pcb->file_array[fd].dev_open)(filename);
	}
	
	else{
		if(read_dentry_by_name(filename, &dentry) == -1){
			//no matching file
			return -1;
		}
		if(dentry.file_type == 1){
			//this is a directory
			curr_pcb->file_array[fd].dev_open = dir_open;
			curr_pcb->file_array[fd].dev_close = dir_close;
			curr_pcb->file_array[fd].dev_read = dir_read;
			curr_pcb->file_array[fd].dev_write = dir_write;
			curr_pcb->file_array[fd].inode = 0;				//unused
			curr_pcb->file_array[fd].file_pos = 0; 			//unused
			curr_pcb->file_array[fd].present = 1;
			
			// run directory open now
			(*curr_pcb->file_array[fd].dev_open)(filename);
		}
		else if(dentry.file_type == 2){
			//this is a normal file
			curr_pcb->file_array[fd].dev_open = file_open;
			curr_pcb->file_array[fd].dev_close = file_close;
			curr_pcb->file_array[fd].dev_read = file_read;
			curr_pcb->file_array[fd].dev_write = file_write;
			curr_pcb->file_array[fd].inode = dentry.inode_num;
			curr_pcb->file_array[fd].file_pos = 0;			//starts at 0
			curr_pcb->file_array[fd].present = 1;
			
			// run file open now
			(*curr_pcb->file_array[fd].dev_open)(filename);
		}
		else{
			//invalid file type
			return -1;
		}
	}
	return fd;
}

/* 
 * __sys_close__
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
	// check that the file descriptor is valid
	if(fd < 0 || fd >= MAX_FILE){
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

int32_t sys_getargs(uint8_t* buf, int32_t nbytes){
	return 0;
}

int32_t sys_vidmap(uint8_t** screen_start){
	return -1;
}

int32_t sys_sethandler(int32_t signum, void* handler_address){
	return -1;
}

int32_t sys_sigreturn(void){
	return -1;
}

int32_t exe_check(uint8_t* name_buf){
	uint8_t file_start[4];			// first byte is null then next 3 should be "ELF"
	int32_t resp;
	
	// check that file exists and has ELF at the start
	resp = file_open(name_buf);
	if(resp != 0)
		return -1;
	resp = file_read(0, file_start, 4);
	if(resp != 4)
		return -1;
	if(strncmp(file_start + 1, (uint8_t*)"ELF", 3) != 0)
		return -1;
	//return 0 on success
	file_close(0);
	return 0;
}
