
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
	int i;
	
	//start with every element in the directory and table set to 0 and overwrite the important ones afterwards
	for(i=0; i<PAGE_ENTRY_NUM; i++){
		page_dir[i].val = 0;
		page_table[i].val = 0;
	}
	
	// make a pde for the currently allocated page table (directory index 0 because this is for the 0 to 4MB region)
	page_dir[0].table_addr = (((unsigned int)page_table) >> FOUR_KB_SHIFT);
	page_dir[0].available = 0;			// we are not using these bits
	page_dir[0].ignored = 0;
	page_dir[0].page_size = 0;			// 0 because this corresponds to a page table
	page_dir[0].zero_pad = 0;
	page_dir[0].accessed = DISABLE;
	page_dir[0].cache_dis = DISABLE;
	page_dir[0].write_thru = DISABLE;
	page_dir[0].user_super = SUPER;
	page_dir[0].read_write = ENABLE;
	page_dir[0].present = ENABLE;
	
	// make a 4kB page in the page table for the video memory
	page_table[VIDEO_PAGE_NUM].page_addr = (VIDEO_START >> FOUR_KB_SHIFT);
	page_table[VIDEO_PAGE_NUM].available = 0;		//we are not using these bits
	page_table[VIDEO_PAGE_NUM].global = ENABLE;
	page_table[VIDEO_PAGE_NUM].zero_pad = 0;
	page_table[VIDEO_PAGE_NUM].dirty = DISABLE;
	page_table[VIDEO_PAGE_NUM].accessed = DISABLE;
	page_table[VIDEO_PAGE_NUM].cache_dis = DISABLE;
	page_table[VIDEO_PAGE_NUM].write_thru = DISABLE;
	page_table[VIDEO_PAGE_NUM].user_super = SUPER;
	page_table[VIDEO_PAGE_NUM].read_write = ENABLE;
	page_table[VIDEO_PAGE_NUM].present = ENABLE;
	
	// make a 4MB page in the page directory for the kernel (directory index 1 because it is the 4MB to 8MB region)
	page_dir[1].table_addr = (KERNEL_START >> FOUR_KB_SHIFT);
	page_dir[1].available = 0;			// we are not using these bits
	page_dir[1].ignored = 0;
	page_dir[1].page_size = 1;			// 1 because this corresponds to a 4MB page
	page_dir[1].zero_pad = 0;
	page_dir[1].accessed = DISABLE;
	page_dir[1].cache_dis = DISABLE;
	page_dir[1].write_thru = DISABLE;
	page_dir[1].user_super = SUPER;
	page_dir[1].read_write = ENABLE;
	page_dir[1].present = ENABLE;
}
