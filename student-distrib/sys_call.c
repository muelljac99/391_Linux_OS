
#include "types.h"
#include "sys_call.h"
#include "lib.h"
#include "rtc.h"
#include "terminal.h"
#include "file_dir.h"

int32_t sys_halt(uint8_t status){
	return -1;
}

int32_t sys_execute(const uint8_t* command){
	uint32_t command_len = strlen((int8_t*) command);
	uint32_t word_flag = 0;
	uint32_t word_break;
	uint8_t exe[ARG_BUF_SIZE] = "";
	uint8_t args[ARG_BUF_SIZE] = "";
	int i;
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
	return -1;
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
	// check that the file descriptor is valid
	if(fd < 0 || fd >= MAX_FILE){
		return -1;
	}
	else if(file_array[fd].present == 0){
		return -1;
	}
	else{
		// if it is valid then run the corresponding read function
		return (*file_array[fd].dev_read)(fd, buf, nbytes);
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
	// check that the file descriptor is valid
	if(fd < 0 || fd >= MAX_FILE){
		return -1;
	}
	else if(file_array[fd].present == 0){
		return -1;
	}
	else{
		// if it is valid then run the corresponding write function
		return (*file_array[fd].dev_write)(fd, buf, nbytes);
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
	dentry_t dentry;
	unsigned char rtc[RTC_NAME_LEN] = RTC_NAME;
	unsigned char term[TERMINAL_NAME_LEN] = TERMINAL_NAME;
	
	//find a spot in the file array
	for(fd = 0; fd < MAX_FILE; fd++){
		if(file_array[fd].present == 0){
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
		file_array[fd].dev_open = rtc_open;
		file_array[fd].dev_close = rtc_close;
		file_array[fd].dev_read = rtc_read;
		file_array[fd].dev_write = rtc_write;
		file_array[fd].inode = 0; 				//unused
		file_array[fd].file_pos = 0;			//unused
		file_array[fd].present = 1;				
		
		//run rtc open now
		(*file_array[fd].dev_open)(filename);
	}
	
	else if(strncmp(filename, term, TERMINAL_NAME_LEN) == 0){
		// set up the entry for the terminal
		file_array[fd].dev_open = terminal_open;
		file_array[fd].dev_close = terminal_close;
		file_array[fd].dev_read = terminal_read;
		file_array[fd].dev_write = terminal_write;
		file_array[fd].inode = 0;				//unused
		file_array[fd].file_pos = 0; 			//unused
		file_array[fd].present = 1;
		
		// run terminal open now
		(*file_array[fd].dev_open)(filename);
	}
	
	else{
		if(read_dentry_by_name(filename, &dentry) == -1){
			//no matching file
			return -1;
		}
		if(dentry.file_type == 1){
			//this is a directory
			file_array[fd].dev_open = dir_open;
			file_array[fd].dev_close = dir_close;
			file_array[fd].dev_read = dir_read;
			file_array[fd].dev_write = dir_write;
			file_array[fd].inode = 0;				//unused
			file_array[fd].file_pos = 0; 			//unused
			file_array[fd].present = 1;
			
			// run directory open now
			(*file_array[fd].dev_open)(filename);
		}
		else if(dentry.file_type == 2){
			//this is a normal file
			file_array[fd].dev_open = file_open;
			file_array[fd].dev_close = file_close;
			file_array[fd].dev_read = file_read;
			file_array[fd].dev_write = file_write;
			file_array[fd].inode = dentry.inode_num;
			file_array[fd].file_pos = 0;			//starts at 0
			file_array[fd].present = 1;
			
			// run file open now
			(*file_array[fd].dev_open)(filename);
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
	uint32_t ret_val;
	// check that the file descriptor is valid
	if(fd < 0 || fd >= MAX_FILE){
		return -1;
	}
	else if(file_array[fd].present == 0){
		return -1;
	}
	else{
		// if it is valid then run the specific close function
		ret_val = (*file_array[fd].dev_close)(fd);
		
		// empty the entry in the array
		file_array[fd].dev_open = NULL;
		file_array[fd].dev_close = NULL;
		file_array[fd].dev_read = NULL;
		file_array[fd].dev_write = NULL;
		file_array[fd].inode = 0;				//unused
		file_array[fd].file_pos = 0; 			//unused
		file_array[fd].present = 0;
		
		//return with the value from the specific function
		return ret_val;
	}
}

int32_t sys_getargs(uint8_t* buf, int32_t nbytes){
	return -1;
}

int32_t sys_vidmap(uint8_t** screen_start){
	return -1;
}

int32_t set_handler(int32_t signum, void* handler_address){
	return -1;
}

int32_t sigreturn(void){
	return -1;
}
