
#include "multiboot.h"
#include "x86_desc.h"
#include "lib.h"
#include "i8259.h"
#include "irq_asm.h"
#include "service_irq.h"
#include "sys_call.h"
#include "file_dir.h"

dentry_t file_dentry;
uint32_t file_pos = 0;

uint32_t dir_index;

int32_t file_open(const uint8_t* filename){
	read_dentry_by_name(filename, &file_dentry);
	file_pos = 0;
	return 0;
}

int32_t file_close(int32_t fd){
	file_pos = 0;
	return 0;
}

int32_t file_read(int32_t fd, void* buf, int32_t nbytes){
	uint8_t* read_buf = (uint8_t*)buf;
	uint32_t read_len;
	//check that buffer is valid
	if(read_buf == NULL){
		return -1;
	}
	read_len = read_data(file_dentry.inode_num, file_pos, read_buf, nbytes);
	file_pos += read_len;
	return read_len;
}

int32_t file_write(int32_t fd, const void* buf, int32_t nbytes){
	//do nothing
	return -1;
}

int32_t dir_open(const uint8_t* filename){
	dir_index = 0;
	return 0;
}

int32_t dir_close(int32_t fd){
	dir_index = 0;
	return 0;
}

int32_t dir_read(int32_t fd, void* buf, int32_t nbytes){
	uint8_t info[80] = "file_name:                                  |file_type:   |file_size:       \n";
	uint8_t* read_buf = (uint8_t*)buf;
	dentry_t dir_entry;
	uint32_t num_dir;
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
	for(i=11; i<11+NAME_LEN; i++){
		info[i] = dir_entry.filename[i-11];
	}
	//fill the file type into the buffer
	info[56] = dir_entry.file_type + '0';
	//fill the file size into the buffer
	if(dir_entry.file_type == 2){
		
		itoa((*(file_sys_addr + ((dir_entry.inode_num + 1)*(INODE_SIZE/4)))), (int8_t*)(info+70), 10);
	}
	else{
		info[70] = '0';
	}
	
	memcpy(read_buf, info, 80);
	
	return 0;
}

int32_t dir_write(int32_t fd, const void* buf, int32_t nbytes){
	//do nothing
	return -1;
}

int32_t read_dentry_by_name(const uint8_t* fname, dentry_t* dentry){
	uint8_t filename[32] = "";
	uint32_t num_dir;
	int i;
	uint32_t comp_len;
	dentry_t* curr;
	
	//check for valid name and directory entry ptr
	if((fname == NULL)||(dentry == NULL)){
		return -1;
	}
	
	if(strlen((int8_t*)fname) < NAME_LEN){
		comp_len = strlen((int8_t*)fname);
	}
	else{
		comp_len = NAME_LEN;
	}
	
	//remove any newline characters from the name
	for(i=0; i<comp_len; i++){
		if(fname[i] == '\n'){
			filename[i] = '\0';
		}
		else{
			filename[i] = fname[i];
		}
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

int32_t read_dentry_by_index(uint32_t index, dentry_t* dentry){
	uint32_t num_dir;
	dentry_t* curr;
	
	if(dentry==NULL){
		return -1;
	}
	
	//get the first directory entry
	num_dir = (*file_sys_addr);
	curr = (dentry_t*)(file_sys_addr + (DENTRY_SIZE/4));
	
	if(index >= num_dir){
		return -1;
	}
	curr += index;
	//copy the entire directory entry structure to the dentry ptr
	memcpy(dentry, curr, DENTRY_SIZE);
	return 0;
}

int32_t read_data(uint32_t inode, uint32_t offset, uint8_t* buf, uint32_t length){
	uint32_t num_inodes, num_data_blocks;
	uint32_t data_len, data_block_num;
	uint32_t* inode_ptr;
	uint32_t* data_block_start;
	uint8_t* block_ptr;
	uint32_t data_read = 0;
	uint32_t section_length;
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
		if((length < (data_len-offset)) && (length < (DATA_BLOCK_SIZE-(offset%DATA_BLOCK_SIZE)))){
			//requested read length is first barrier
			section_length = length;
		}
		else if(((data_len-offset) < length)&&((data_len-offset) < (DATA_BLOCK_SIZE-(offset%DATA_BLOCK_SIZE)))){
			//end of file is the first barrier
			section_length = data_len - offset;
		}
		else{
			//end of data block is the first barrier
			section_length = DATA_BLOCK_SIZE-(offset%DATA_BLOCK_SIZE);
		}
		//copy up until the first barrier
		for(i=0; i<section_length; i++){
			buf[i] = block_ptr[i];
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
