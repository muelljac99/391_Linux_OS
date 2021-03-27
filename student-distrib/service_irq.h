
#ifndef _SERVICE_IRQ_H
#define _SERVICE_IRQ_H

#include "types.h"

/* exception number info */
#define NUM_EXCEPTION	32

/* the number of PIC irq lines */
#define PIC_IRQ_NUM		16

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

#endif /* _SERVICE_IRQ_H */
