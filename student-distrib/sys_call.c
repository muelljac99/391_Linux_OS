
#include "types.h"
#include "sys_call.h"
#include "lib.h"

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

int32_t __sys_open__(const uint8_t* filename){
	/*
	int fd;
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
	if(strncmp(filename, RTC_NAME, 3) == 0){
		// set up the entry for the rtc
		file_array[fd].dev_open =
		file_array[fd].dev_close = 
		file_array[fd].dev_read = 
		file_array[fd].dev_write = 
		file_array[fd].inode = 0; 				//unused
		file_array[fd].file_pos = 0;			//unused
		file_array[fd].present = 1;				
		
		//run rtc open now
		(*file_array[fd].dev_open)(filename);
	}
	
	*/
	/* do this for terminal, file, and directory */
	return 0;
}

int32_t __sys_close__(int32_t fd){
	return 0;
}

int32_t __sys_read__(int32_t fd, void* buf, int32_t nbytes){
	return 0;
}

int32_t __sys_write__(int32_t fd, const void* buf, int32_t nbytes){
	return 0;
}

