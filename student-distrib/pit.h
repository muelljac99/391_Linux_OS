
#ifndef _PIT_H
#define _PIT_H

#include "types.h"

/* PIT IRQ and port info */
#define PIT_IRQ			0x20
#define PIT_DATA		0x40
#define PIT_CMD			0x43

void init_pit(void);

void handle_pit(void);

#endif /* _PIT_H */
