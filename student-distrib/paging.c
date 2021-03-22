
#include "multiboot.h"
#include "x86_desc.h"
#include "lib.h"
#include "i8259.h"
#include "irq_asm.h"
#include "service_irq.h"
#include "paging.h"

/* 
 * load_page
 *   DESCRIPTION: Fills the page directory with the first page table and a 4MB page for the kernel.
 * 				  Fills the page table with a 4kB page for the video memory. All other pages are not present.
 *   INPUTS: none
 *   OUTPUTS: none
 *   RETURN VALUE: none
 *   SIDE EFFECTS: changes the page directory and page table
 */
void load_page(void){
	// make a pde for the currently allocated page table (anything with a VM addr less than 4MB goes here)
	page_dir[0].table_addr = (((unsigned int)page_table) >> 12);
	page_dir[0].page_size = 0;
	page_dir[0].user_super = 0;
	page_dir[0].read_write = 1;
	page_dir[0].present = 1;
	// make a 4kB page in the page table for the video memory (the rest are not present)(starts at 0xB8000 and is identity paging )
	page_table[184].page_addr = (0x000B8000 >> 12);
	page_table[184].user_super = 0;
	page_table[184].read_write = 1;
	page_table[184].present = 1;
	// make a 4MB page in the page directory for the kernel (the rest are not present)(this page corresponds to index 1 for identity paging)
	page_dir[1].table_addr = (0x00400000 >> 12);
	page_dir[1].page_size = 1;
	page_dir[1].user_super = 0;
	page_dir[1].read_write = 1;
	page_dir[1].present = 1;
}
