
#include "types.h"
#include "sys_call.h"
#include "lib.h"
#include "rtc.h"
#include "terminal.h"
#include "file_dir.h"

/* 
 * do_sys_call
 *   DESCRIPTION: This is the common C function for all system calls. This function determines which
 * 				  type of call it is and performs any general operation as well as the device specific operations
 *   INPUTS: call_num -- the number that corresponds to which type of system call it is
 * 			 arg1 -- the first possible argument for the call (may or may not be used)
 * 			 arg2 -- the second possible argument for the call (may or may not be used)
 * 			 arg3 -- the third possible argument for the call (may or may not be used)
 *   OUTPUTS: none
 *   RETURN VALUE: 0 on success, -1 on failure
 *   SIDE EFFECTS: may edit file array
 */
int32_t do_sys_call(int call_num, uint32_t arg1, uint32_t arg2, uint32_t arg3){
	switch(call_num){
		case 1:
			//halt
			printf("HALT SYSTEM CALL");
			return -1;
			
		case 2:
			//execute
			printf("EXECUTE SYSTEM CALL");
			return -1;
			
		case 3:
			//read
			return __sys_read__((int32_t)arg1, (void*)arg2, (int32_t)arg3);
			
		case 4:
			//write
			return __sys_write__((int32_t)arg1, (void*)arg2, (int32_t)arg3);
			
		case 5:
			//open
			return __sys_open__((uint8_t*)arg1);
			
		case 6:
			//close
			return __sys_close__((int32_t)arg1);
			
		case 7:
			//getargs
			printf("GETARGS SYSTEM CALL");
			return -1;
			
		case 8:
			//vidmap
			printf("VIDMAP SYSTEM CALL");
			return -1;
			
		case 9:
			//set_handler
			printf("SET_HANDLER SYSTEM CALL");
			return -1;
			
		case 10:
			//sigreturn
			printf("SIGRETURN SYSTEM CALL");
			return -1;
			
			
		default:
			printf("INVALID SYSTEM CALL NUMBER: %x\n", call_num);
			return -1;
			
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
int32_t __sys_open__(const uint8_t* filename){
	int fd;
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
		// if not those two devices then it is a file or directory in file system
		// ****************
	}
	
	return 0;
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
int32_t __sys_close__(int32_t fd){
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
int32_t __sys_read__(int32_t fd, void* buf, int32_t nbytes){
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
int32_t __sys_write__(int32_t fd, const void* buf, int32_t nbytes){
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

