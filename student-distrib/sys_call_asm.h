
#ifndef _SYS_CALL_ASM_H
#define _SYS_CALL_ASM_H

#include "types.h"

/* user context info */
#define USER_STACK		0x08400000
#define INT_FLAG		0x00000200

#ifndef ASM

/* helper assembly function to clear the tlb */
extern void tlb_flush();

/* helper assembly function to get the esp ptr */
extern uint32_t get_esp();

/* helper assembly function to get the ebp ptr */
extern uint32_t get_ebp();

/* helper assembly function to push the user info for an iret */
extern uint32_t push_context(uint32_t start);

/* helper assembly function to jump from halt back to the exceute of the parent process */
extern void halt_jump(uint32_t ret_val, uint32_t parent_stack_ptr);

/* helper function that will set the esp and ebp to the passed in values to switch to a new kernel stack */
extern void switch_stack(uint32_t esp, uint32_t ebp);

#endif /* ASM */

#endif /* _SYS_CALL_ASM_H */
