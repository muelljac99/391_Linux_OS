# vim:ts=4 noexpandtab

#define ASM     1
#include "sys_call_asm.h"
#include "sys_call.h"
#include "x86_desc.h"

.text

# globals for the function names to be used elsewhere
.global tlb_flush, get_esp, get_ebp, push_context, halt_jump, switch_stack

/* 
 * tlb_flush
 *   DESCRIPTION: An assemby helper function that clears the tlb by rewriting the cr3 with page directory pointer
 *   INPUTS: none
 *   OUTPUTS: none
 *   RETURN VALUE: none
 *   SIDE EFFECTS: flushes tlb
 */
tlb_flush:
	# push ebp
	pushl %ebp
	movl %esp, %ebp
	
	# save the callee save registers and flags
	pushfl
	pushl %eax
	
	# set the cr3 with the page directory pointer
	movl %cr3, %eax
	movl %eax, %cr3
	# this will flush the TLB
	
	# restore callee save registers and flags
	popl %eax
	popfl
	
	leave
	ret

/* 
 * get_esp
 *   DESCRIPTION: An assemby helper function that returns the current stack pointer
 *   INPUTS: none
 *   OUTPUTS: none
 *   RETURN VALUE: esp value
 *   SIDE EFFECTS: none
 */
get_esp:
	# push ebp
	pushl %ebp
	movl %esp, %ebp
	
	# put the esp into the return spot
	movl %esp, %eax
	
	leave
	ret

/* 
 * get_ebp
 *   DESCRIPTION: An assemby helper function that returns the current stack base pointer
 *   INPUTS: none
 *   OUTPUTS: none
 *   RETURN VALUE: ebp value
 *   SIDE EFFECTS: none
 */
get_ebp:
	# put the base pointer into the return spot
	movl %ebp, %eax

	ret

/* 
 * push context
 *   DESCRIPTION: An assemby helper function that pushes the user context for an iret call. This function also
 *				  receives a jump from halt system calls to conclude execute system calls
 *   INPUTS: start -- the starting eip for a user space program
 *   OUTPUTS: none
 *   RETURN VALUE: none
 *   SIDE EFFECTS: switches context and clobbers eax
 */
push_context:
	# save the ebp for the system execute function
	pushl %ebp
	#push user ds
	pushl $USER_DS
	#push ESP (user stack at 132MB)
	movl $USER_STACK, %eax
	subl $4, %eax
	pushl %eax
	#push EFLAG with interrupts enabled
	sti
	pushfl
	#push user cs
	pushl $USER_CS
	#push EIP (start of program image) -> the argument of the function
	movl 24(%esp), %eax
	pushl %eax
	
	#context filled in so now IRET
	iret

execute_return:
	# restore the ebp to the execute base pointer
	popl %ebp
	
	ret

/* 
 * halt_jump
 *   DESCRIPTION: An assemby helper function that returns control to the parent process and parent kernel stack and
 *				  jumps back to the execute system call to return to the parent user program 
 *   INPUTS: ret_val -- the status value for a halt system call that is returned by execute
 *			 parent_stack_ptr -- ptr to the parent kernel stack to be restored
 *   OUTPUTS: none
 *   RETURN VALUE: none
 *   SIDE EFFECTS: switches process and stack and jumps to execute_return
 */
halt_jump:
	# put the return value into EBX
	movl 4(%esp), %eax
	
	# restore the parent stack pointer
	movl 8(%esp), %esp
	
	# jump to the execute return
	jmp execute_return

/*
 * switch_stack
 */
switch_stack:
	#update the ebp and esp
	movl 8(%esp), %ebp
	movl 4(%esp), %esp
	#return now that we are in the new stack
	leave
	ret
	
	
	

