
#ifndef _SYS_CALL_ASM_H
#define _SYS_CALL_ASM_H

#include "types.h"

/* user context info */
#define USER_STACK		0x08400000

#ifndef ASM

/* helper assembly function to clear the tlb */
extern void tlb_flush();

/* helper assembly function to get the esp ptr */
extern uint32_t get_esp();

/* helper assembly function to push the user info for an iret */
extern void push_context();

#endif /* ASM */

#endif /* _SYS_CALL_ASM_H */
