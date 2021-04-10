# vim:ts=4 noexpandtab

#define ASM     1
#include "sys_call_asm.h"
#include "sys_call.h"
#include "x86_desc.h"

.text

.global tlb_flush, get_esp, push_context, halt_jump

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
	
get_esp:
	# push ebp
	pushl %ebp
	movl %esp, %ebp
	
	# put the esp into the return spot
	movl %esp, %eax
	
	leave
	ret
	
push_context:
	#push user ds
	pushl $USER_DS
	#push ESP (user stack at 132MB)
	movl $USER_STACK, %eax
	subl $4, %eax
	pushl %eax
	#push EFLAG
	pushfl
	#push user cs
	pushl $USER_CS
	#push EIP (start of program image) -> the argument of the function
	movl 20(%esp), %eax
	pushl %eax
	
	#context filled in so now IRET
	iret

execute_return:
	# remove the user ds value from the stack
	addl $4, %esp
	#normal leave
	ret
	
halt_jump:
	# push ebp
	pushl %ebp
	movl %esp, %ebp
	
	# put the return value into EBX
	movl 8(%ebp), %eax
	
	# restore the parent stack pointer
	movl 12(%ebp), %esp
	
	# jump to the execute return
	jmp execute_return
