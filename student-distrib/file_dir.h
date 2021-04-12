
#ifndef _FILE_DIR_H
#define _FILE_DIR_H

#include "types.h"

/* file system info definitions */
#define NAME_LEN 			32
#define DENTRY_SIZE 		64
#define INODE_SIZE			4096
#define DATA_BLOCK_SIZE		4096

#define TYPE_FILE			2
#define TYPE_DIR			1

/* directory read output constants */
#edfine FILENAME_POS 		11
#define DIR_READ_BUF_SIZE   80

/* file system address global */
uint32_t* file_sys_addr;

/* directory entry structure */
typedef struct dentry {
	uint8_t filename[NAME_LEN];
	uint32_t file_type;
	uint32_t inode_num;
	uint32_t reserved[6];
} dentry_t;

/* the file driver open function */
int32_t file_open(const uint8_t* filename);

/* the file driver close function */
int32_t file_close(int32_t fd);

/* the file driver read function */
int32_t file_read(int32_t fd, void* buf, int32_t nbytes);

/* the file driver write function */
int32_t file_write(int32_t fd, const void* buf, int32_t nbytes);

/* the directory driver open function */
int32_t dir_open(const uint8_t* filename);

/* the directory driver close function */
int32_t dir_close(int32_t fd);

/* the directory driver read function */
int32_t dir_read(int32_t fd, void* buf, int32_t nbytes);

/* the directory driver write function */
int32_t dir_write(int32_t fd, const void* buf, int32_t nbytes);

/* the helper function to get a directory entry based on a name input */
int32_t read_dentry_by_name(const uint8_t* fname, dentry_t* dentry);

/* the helper function to get a directory entry based on an index input */
int32_t read_dentry_by_index(uint32_t index, dentry_t* dentry);

/* the helper function to get data from a file inode */
int32_t read_data(uint32_t inode, uint32_t offset, uint8_t* buf, uint32_t length);

#endif /* _FILE_DIR_H */
