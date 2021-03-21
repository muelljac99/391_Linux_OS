
#ifndef _SET_IDT_H
#define _SET_IDT_H

#include "types.h"

/* IDT Constants */
#define BASE_INT		32
#define SYS_CALL_IRQ	0x80


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

/* the function to fill the idt entries at boot */
void idt_fill(void);

uint32_t do_irq(pt_regs_t* reg);


#endif /* _SET_IDT_H */
