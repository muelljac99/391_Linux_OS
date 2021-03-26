
#ifndef _SERVICE_IRQ_H
#define _SERVICE_IRQ_H

#include "types.h"

/* video memory size definitions */
#define NUM_COLS    80
#define NUM_ROWS    25

/* exception number info */
#define NUM_EXCEPTION	32

/* the number of PIC irq lines */
#define PIC_IRQ_NUM		16

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

/* the structure to hold the saved registers when performing do_irq */
typedef struct pt_regs {
	uint32_t EBX;
	uint32_t ECX;
	uint32_t EDX;
	uint32_t ESI;
	uint32_t EDI;
	uint32_t EBP;
	uint32_t EAX;
	uint32_t DS_ext;
	uint32_t ES_ext;
	uint32_t FS_ext;
	uint32_t irq_num;
} pt_regs_t;

/* irq descriptor array holding the handler pointers */
void (*irq_handlers[PIC_IRQ_NUM])();

/* do_irq is the main function called when an interrupt, exception, or system call occurs */
int32_t do_irq(pt_regs_t* reg);

/* handler for the keyboard */
void handle_keyboard(void);

/* initialize the keyboard */
void init_keyboard(void);

#endif /* _SERVICE_IRQ_H */
