
#ifndef _FILE_DIR_H
#define _FILE_DIR_H

#include "types.h"

/* file system info definitions */
#define NAME_LEN 			32
#define DENTRY_SIZE 		64
#define INODE_SIZE			4096
#define DATA_BLOCK_SIZE		4096

/* file system address global */
uint32_t* file_sys_addr;

/* directory entry structure */
typedef struct dentry {
	uint8_t filename[NAME_LEN];
	uint32_t file_type;
	uint32_t inode_num;
	uint32_t reserved[6];
} dentry_t;

int32_t file_open(const uint8_t* filename);

int32_t file_close(int32_t fd);

int32_t file_read(int32_t fd, void* buf, int32_t nbytes);

int32_t file_write(int32_t fd, const void* buf, int32_t nbytes);

int32_t dir_open(const uint8_t* filename);

int32_t dir_close(int32_t fd);

int32_t dir_read(int32_t fd, void* buf, int32_t nbytes);

int32_t dir_write(int32_t fd, const void* buf, int32_t nbytes);

int32_t read_dentry_by_name(const uint8_t* fname, dentry_t* dentry);

int32_t read_dentry_by_index(uint32_t index, dentry_t* dentry);

int32_t read_data(uint32_t inode, uint32_t offset, uint8_t* buf, uint32_t length);

#endif /* _FILE_DIR_H */
