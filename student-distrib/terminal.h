
#ifndef _TERMINAL_H
#define _TERMINAL_H

#include "types.h"

/* terminal buffer info */
#define BUF_SIZE_MAX 	128
#define NUM_TERMINAL	3

/* video memory size definitions */
#define NUM_COLS    	80
#define NUM_ROWS    	25

/* the keyboard irq and port */
#define KEYBOARD_IRQ 	0x21
#define KEYBOARD_PORT 	0x60

/* important scancodes */
#define LSHIFT			0x2A
#define LSHIFT_R		0xAA
#define RSHIFT			0x36
#define RSHIFT_R		0xB6
#define CTRL			0x1D
#define CTRL_R			0x9D
#define ALT				0x38
#define ALT_R			0xB8
#define CAPS_LOCK		0x3A
#define TAB				0x0F
#define ENTER			0x1C
#define BACKSPACE		0x0E
#define F1				0x3B
#define F2				0x3C
#define F3				0x3D

// this is the structure unique to each terminal allowing switching between them
typedef struct term_info {
	int cursor_x;
	int cursor_y;
	uint32_t user_process_num;
	uint32_t init;
	uint32_t esp_save;
	uint32_t ebp_save;
	volatile uint8_t key_buf[BUF_SIZE_MAX];
	volatile int key_buf_size;
	volatile int terminal_flag;
} term_info_t;

/* saved info for each terminal */
term_info_t term_save[NUM_TERMINAL];

/* current visible terminal number */
uint32_t curr_term;

/* handler for the keyboard */
void handle_keyboard(void);

/* initialize the keyboard */
void init_keyboard(void);

/* the terminal driver open function */
int32_t terminal_open(const uint8_t* filename);

/* the terminal driver close function */
int32_t terminal_close(int32_t fd);

/* the terminal driver read function */
int32_t terminal_read(int32_t fd, void* buf, int32_t nbytes);

/* the terminal driver write function */
int32_t terminal_write(int32_t fd, const void* buf, int32_t nbytes);

/* the function to initialize the 3 separate terminals */
void init_terminals(void);

/* function to switch between terminals */
int32_t switch_terminals(uint32_t term_num);

/* function to switch the currently executing process */
int32_t switch_active(uint32_t term_num);

/* the helper function used to write to the keyboard buffer */
void buf_fill(unsigned char add);

#endif /* _TERMINAL_H */
