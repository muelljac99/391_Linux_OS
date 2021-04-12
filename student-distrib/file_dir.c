
#include "multiboot.h"
#include "x86_desc.h"
#include "lib.h"
#include "i8259.h"
#include "irq_asm.h"
#include "service_irq.h"
#include "sys_call.h"
#include "file_dir.h"

// the directory entry and file position for an open file
dentry_t file_dentry;		// this might not let you open more than one file at once
uint32_t file_pos = 0;

// the current entry within the directory that the dir_read will gather the data for
uint32_t dir_index;

/* 
 * file_open
 *   DESCRIPTION: The file driver open function that finds a directory entry by the name of the file and
 * 				  saves the directory entry structure for that file
 *   INPUTS: filename -- the name of the file to be found
 *   OUTPUTS: none
 *   RETURN VALUE: 0 on success, -1 on fail
 *   SIDE EFFECTS: sets the read location to start at the beginning of the file
 */
int32_t file_open(const uint8_t* filename){
	//int8_t* filename_copy = filename;
	
	if (-1 == read_dentry_by_name(filename, &file_dentry)) {
		return -1;
	}
	//check that the requested is a file
	if(file_dentry.file_type != TYPE_FILE){
		return -1;
	}
	file_pos = 0;
	return 0;
}

/* 
 * file_close
 *   DESCRIPTION: The file driver close function that clears the file position
 *   INPUTS: fd -- unused
 *   OUTPUTS: none
 *   RETURN VALUE: 0 on success, -1 on fail
 *   SIDE EFFECTS: none
 */
int32_t file_close(int32_t fd){
	file_pos = 0;
	return 0;
}

/* 
 * file_read
 *   DESCRIPTION: The file driver read function that starts reading at file_pos and reads either the requested
 * 				  number of bytes or until reaching the end of the file.
 *   INPUTS: fd -- unused
 *			 nbytes -- the number of bytes to read from the file
 *   OUTPUTS: buf -- the buffer that the function will write the file data into
 *   RETURN VALUE: number of bytes read from the file
 *   SIDE EFFECTS: sets the read location to where this read has left off in the file
 */
int32_t file_read(int32_t fd, void* buf, int32_t nbytes){
	uint8_t* read_buf = (uint8_t*)buf;
	uint32_t read_len;
	//check that the file has already been opened and is correct type
	if(file_dentry.file_type != TYPE_FILE){
		return -1;
	}
	//check that buffer is valid
	if(read_buf == NULL){
		return -1;
	}
	read_len = read_data(file_dentry.inode_num, file_pos, read_buf, nbytes);
	file_pos += read_len;
	return read_len;
}

/* 
 * file_write
 *   DESCRIPTION: The file driver write function that does nothing
 *   INPUTS: fd -- unused
 * 			 buf -- unused
 * 			 nbytes -- unused
 *   OUTPUTS: none
 *   RETURN VALUE: 0 on success, -1 on fail
 *   SIDE EFFECTS: none
 */
int32_t file_write(int32_t fd, const void* buf, int32_t nbytes){
	//do nothing
	return -1;
}

/* 
 * dir_open
 *   DESCRIPTION: The directory driver open function that initializes the directory index from which to read to 0
 *   INPUTS: filename -- unused
 *   OUTPUTS: none
 *   RETURN VALUE: 0 on success, -1 on fail
 *   SIDE EFFECTS: none
 */
int32_t dir_open(const uint8_t* filename){
	dir_index = 0;
	return 0;
}

/* 
 * dir_close
 *   DESCRIPTION: The directory driver close function that resets the directory index from which to read to 0
 *   INPUTS: fd-- unused
 *   OUTPUTS: none
 *   RETURN VALUE: 0 on success, -1 on fail
 *   SIDE EFFECTS: none
 */
int32_t dir_close(int32_t fd){
	dir_index = 0;
	return 0;
}

/* 
 * dir_read
 *   DESCRIPTION: The directory driver read function that reads the data from the directory entry at dir_index
 * 				  and puts the file information into a readable format into the passed in buffer
 *   INPUTS: fd -- unused
 * 			 nbytes -- unused
 *   OUTPUTS: buf -- the buffer which the function will write the file information into
 *   RETURN VALUE: 0 on success, -1 on fail
 *   SIDE EFFECTS: always writes 80 characters into the buffer
 */
int32_t dir_read(int32_t fd, void* buf, int32_t nbytes){
	uint8_t info[DIR_READ_BUF_SIZE] = "file_name:                                  |file_type:   |file_size:       \n";
	uint8_t* read_buf = (uint8_t*)buf;
	dentry_t dir_entry;
	uint32_t num_dir;
	uint32_t read_size;
	int i;
	
	//check for valid buffer ptr
	if(read_buf == NULL){
		return -1;
	}
	
	num_dir = (*file_sys_addr);
	if(dir_index >= num_dir){
		return 0;
	}
	read_dentry_by_index(dir_index, &dir_entry);
	dir_index++;
	
	//fill the filename into the info buffer
	for(i=FILENAME_POS; i<FILENAME_POS+NAME_LEN; i++){
		// the filename begins at the 11th character in the output buffer
		info[i] = dir_entry.filename[i-FILENAME_POS];
	}
	//fill the file type into the buffer (placed at the 56th character in the output buffer)
	info[56] = dir_entry.file_type + '0';
	//fill the file size into the buffer
	if(dir_entry.file_type == TYPE_FILE){
		//if this is a file type then there will be a file size which is placed at the 70th character
		itoa((*(file_sys_addr + ((dir_entry.inode_num + 1)*(INODE_SIZE/4)))), (int8_t*)(info+70), 10);
	}
	else{
		// if not a file type then there is no size of the file
		info[70] = '0';
	}
	
	//get the size of the read
	if(nbytes < DIR_READ_BUF_SIZE){
		read_size = nbytes;
	}
	else{
		read_size = DIR_READ_BUF_SIZE;
	}
	
	//put the info into the output buffer
	memcpy(read_buf, info, read_size);
	
	return read_size;
}

/* 
 * dir_write
 *   DESCRIPTION: The directory driver write function that does nothing
 *   INPUTS: fd -- unused
 *			 buf -- unused
 * 			 nbytes -- unused
 *   OUTPUTS: none
 *   RETURN VALUE: 0 on success, -1 on fail
 *   SIDE EFFECTS: none
 */
int32_t dir_write(int32_t fd, const void* buf, int32_t nbytes){
	//do nothing
	return -1;
}

/* 
 * read_dentry_by_name
 *   DESCRIPTION: The file driver helper function that gets a directory entry from a name input
 *   INPUTS: fname -- the name of the desired file
 *   OUTPUTS: dentry -- the directory entry structure where the file info is placed
 *   RETURN VALUE: 0 on success, -1 on fail
 *   SIDE EFFECTS: none
 */
int32_t read_dentry_by_name(const uint8_t* fname, dentry_t* dentry){
	uint8_t filename[2*NAME_LEN] = "";					//the copied fname that can be edited
	uint32_t num_dir;									//the total number of directory entries
	int i;
	uint32_t comp_len;									//the length of the file name to be compared
	dentry_t* curr;
	
	//check for valid name and directory entry ptr
	if((fname == NULL)||(dentry == NULL)){
		return -1;
	}
	
	
	//get the minimum of the filename length and the max 32 bytes
	if(strlen((int8_t*)fname) < 2*NAME_LEN){
		comp_len = strlen((int8_t*)fname);
	}
	else{
		comp_len = 2*NAME_LEN;
	}
	
	//remove any newline characters from the name
	for(i=0; i<comp_len; i++){
		if(fname[i] == '\n'){
			filename[i] = '\0';
			// Limit length of filename to i, the location of the first newline character.
			comp_len = i;
			break;
		}
		else{
			filename[i] = fname[i];
		}
	}
	
	// Check that length of filename is within NAME_LEN
	if (strlen((int8_t*)filename) > NAME_LEN) {
		printf("NO FILE MATCH FOUND, FILENAME IS TOO LONG!\n");
		return -1;
	}
	
	//get the first directory entry
	num_dir = (*file_sys_addr);
	curr = (dentry_t*)(file_sys_addr + (DENTRY_SIZE/4));
	
	//find entry with same name
	for(i=0; i<num_dir; i++){
		if(strncmp(filename, curr->filename, comp_len) == 0){
			break;
		}
		else{
			curr++;
		}
	}
	//check if found a match
	if(i == num_dir){
		printf("NO FILE MATCH FOUND\n");
		return -1;
	}
	else{
		//copy the entire directory entry structure to the dentry ptr
		memcpy(dentry, curr, DENTRY_SIZE);
		return 0;
	}
}

/* 
 * read_dentry_by_index
 *   DESCRIPTION: The directory driver helper function that gets a directory entry from a directory index
 *   INPUTS: index -- the index of the requested directory entry
 *   OUTPUTS: dentry -- the directory entry structure where the file info is placed
 *   RETURN VALUE: 0 on success, -1 on fail
 *   SIDE EFFECTS: none
 */
int32_t read_dentry_by_index(uint32_t index, dentry_t* dentry){
	uint32_t num_dir;
	dentry_t* curr;
	
	//check for valid directory entry ptr
	if(dentry==NULL){
		return -1;
	}
	
	//get the first directory entry
	num_dir = (*file_sys_addr);
	curr = (dentry_t*)(file_sys_addr + (DENTRY_SIZE/4));
	
	//check for valid index
	if(index >= num_dir){
		return -1;
	}
	curr += index;
	//copy the entire directory entry structure to the dentry ptr
	memcpy(dentry, curr, DENTRY_SIZE);
	return 0;
}

/* 
 * read_data
 *   DESCRIPTION: The file driver helper function that reads data from a specified inode within the file system
 *   INPUTS: inode -- the inode of the file which data should be read from
 *			 offset -- the offset from the start of the file to start reading
 *			 length -- the number of bytes to read from the file
 *   OUTPUTS: buf -- the buffer which this function writes the file data to
 *   RETURN VALUE: the number of bytes read
 *   SIDE EFFECTS: none
 */
int32_t read_data(uint32_t inode, uint32_t offset, uint8_t* buf, uint32_t length){
	uint32_t num_inodes, num_data_blocks;				//the number of inodes and data blocks in the file system
	uint32_t data_len, data_block_num;					//the total length of the file and the current data block being read from
	uint32_t* inode_ptr;								//the pointer to the inode structure of the requested inode
	uint32_t* data_block_start;							//the pointer to the start of the data blocks in the file system
	uint8_t* block_ptr;									//the pointer to the location in the data blocks currently being read from
	uint32_t data_read = 0;								//the sum of the total data read
	uint32_t section_length;							//the length of a single read before running into a barrier
	int i;
	
	//check for valid buffer
	if(buf == NULL){
		return -1;
	}
	
	//get the number of inodes and data blocks in the file system
	num_inodes = (*(file_sys_addr+1));
	num_data_blocks = (*(file_sys_addr+2));
	
	//check valid inode and get the ptr to the inode
	if(inode >= num_inodes){
		return -1;
	}
	//get a pointer to our inode and the first data block
	inode_ptr = file_sys_addr + ((1+inode)*(INODE_SIZE/4));
	data_block_start = file_sys_addr + ((num_inodes+1)*(INODE_SIZE/4));
	
	//get the length of the data of our file and check the offset is less than this length
	data_len = (*inode_ptr);
	if(offset >= data_len){
		return -1;
	}
	
	while(1){
		// get the current data block number based on the offset and a pointer to this data block
		data_block_num = (*(inode_ptr + (offset/DATA_BLOCK_SIZE)+1));
		block_ptr = (uint8_t*)(data_block_start + (data_block_num*(DATA_BLOCK_SIZE/4)));
		
		//get the starting read location within the data block
		block_ptr += (offset%DATA_BLOCK_SIZE);
		
		//determine the first barrier
		if((length <= (data_len-offset)) && (length <= (DATA_BLOCK_SIZE-(offset%DATA_BLOCK_SIZE)))){
			//requested read length is first barrier
			section_length = length;
		}
		else if(((data_len-offset) <= length)&&((data_len-offset) <= (DATA_BLOCK_SIZE-(offset%DATA_BLOCK_SIZE)))){
			//end of file is the first barrier
			section_length = data_len - offset;
		}
		else{
			//end of data block is the first barrier
			section_length = DATA_BLOCK_SIZE-(offset%DATA_BLOCK_SIZE);
		}
		//copy up until the first barrier
		for(i=0; i<section_length; i++){
			buf[data_read+i] = block_ptr[i];
		}
		length -= section_length;
		offset += section_length;
		data_read += section_length;
		//check if done reading for any reason
		if(length == 0){
			// reached end of requested read length
			return data_read;
		}
		if(data_len-offset <= 1){
			//we are now at the end of the file so nothing more to read
			return data_read;
		}
	}
}
