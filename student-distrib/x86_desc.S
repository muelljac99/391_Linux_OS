# x86_desc.S - Set up x86 segment descriptors, descriptor tables
# vim:ts=4 noexpandtab

#define ASM     1
#include "x86_desc.h"

.text

.globl ldt_size, tss_size
.globl gdt_desc, ldt_desc, tss_desc
.globl tss, tss_desc_ptr, ldt, ldt_desc_ptr
.globl gdt_ptr
.globl idt_desc_ptr, idt
.globl page_dir, page_table
.globl page_enable

.align 4


tss_size:
    .long tss_bottom - tss - 1

ldt_size:
    .long ldt_bottom - ldt - 1
	
	.word 0 # padding
gdt_desc:
	.word gdt_bottom - gdt - 1
	.long gdt

    .word 0 # Padding
ldt_desc:
    .word KERNEL_LDT
    .long ldt

    .align 4
tss:
_tss:
    .rept 104
    .byte 0
    .endr
tss_bottom:

    .align  16
gdt:
_gdt:

    # First GDT entry cannot be used
    .quad 0

    # NULL entry
    .quad 0

    # Segmentation will not be used
    # CS and DS both are 0-4GB r/w segments
    #
    # The layout is (from Intel IA-32 reference manual):
    #  31        24 23  22  21  20  19   16 15  14 13 12  11   8 7          0
    # |----------------------------------------------------------------------|
    # |            |   | D |   | A |  Seg  |   |  D  |   |      |            |
    # | Base 31:24 | G | / | 0 | V | Limit | P |  P  | S | Type | Base 23:16 |
    # |            |   | B |   | L | 19:16 |   |  L  |   |      |            |
    # |----------------------------------------------------------------------|
    #
    # |----------------------------------------------------------------------|
    # |                                    |                                 |
    # | Base 15:0                          | Segment Limit 15:0              |
    # |                                    |                                 |
    # |----------------------------------------------------------------------|

gdt_ptr:
    # Set up an entry for kernel CS
    .quad 0x00CF9A000000FFFF

    # Set up an entry for kernel DS
    .quad 0x00CF92000000FFFF

    # Set up an entry for user CS
    .quad 0x00CFFA000000FFFF

    # Set up an entry for user DS
    .quad 0x00CFF2000000FFFF

    # Set up an entry for TSS
tss_desc_ptr:
    .quad 0

    # Set up one LDT
ldt_desc_ptr:
    .quad 0

gdt_bottom:

    .align 16
ldt:
    .rept 4
    .quad 0
    .endr
ldt_bottom:

.align 4
    .word 0 # Padding
idt_desc_ptr:
    .word idt_bottom - idt - 1
    .long idt


    .align  16
idt:
_idt:
    .rept NUM_VEC
    .quad 0
    .endr

idt_bottom:

#allocating the space for one page table and the page directory
	.align 4096
page_dir:
_page_dir:
	.rept PAGE_ENTRY_NUM
	.long 0
	.endr
page_dir_bottom:
	
	.align 4096
page_table:
_page_table:
	.rept PAGE_ENTRY_NUM
	.long 0
	.endr
page_table_bottom:

/* 
 * page_enable
 *   DESCRIPTION: Set the control registers of the system to enable paging of mixed sizes and
 * 				  set the register for the address of the page directory
 *   INPUTS: none
 *   OUTPUTS: none
 *   RETURN VALUE: none
 *   SIDE EFFECTS: changes control registers
 */
page_enable:
	# push ebp
	pushl %ebp
	movl %esp, %ebp
	
	# save the callee save registers and flags
	pushfl
	pushl %eax
	
	# set the cr3 with the page directory pointer
	movl $page_dir, %eax
	movl %eax, %cr3
	
	# set cr4 (bit 4) to allow mixed page sizes
	movl %cr4, %eax
	orl $0x00000010, %eax
	movl %eax, %cr4
	
	# set cr0 with paging (bit 31) and protection bits (bit 0)
	movl %cr0, %eax
	orl $0x80000001, %eax
	movl %eax, %cr0
	
	# restore callee save registers and flags
	popl %eax
	popfl
	
	leave
	ret
