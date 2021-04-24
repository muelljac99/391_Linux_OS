
#include "multiboot.h"
#include "x86_desc.h"
#include "lib.h"
#include "i8259.h"
#include "irq_asm.h"
#include "service_irq.h"
#include "paging.h"
#include "sys_call.h"

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
	
	// make a 4kB page for the terminal 1 video memory save
	page_table[VIDEO_PAGE_NUM+1].page_addr = (TERM1_VID >> FOUR_KB_SHIFT);
	page_table[VIDEO_PAGE_NUM+1].available = 0;		//we are not using these bits
	page_table[VIDEO_PAGE_NUM+1].global = ENABLE;
	page_table[VIDEO_PAGE_NUM+1].zero_pad = 0;
	page_table[VIDEO_PAGE_NUM+1].dirty = DISABLE;
	page_table[VIDEO_PAGE_NUM+1].accessed = DISABLE;
	page_table[VIDEO_PAGE_NUM+1].cache_dis = DISABLE;
	page_table[VIDEO_PAGE_NUM+1].write_thru = DISABLE;
	page_table[VIDEO_PAGE_NUM+1].user_super = SUPER;
	page_table[VIDEO_PAGE_NUM+1].read_write = ENABLE;
	page_table[VIDEO_PAGE_NUM+1].present = ENABLE;
	
	// make a 4kB page for the terminal 2 video memory save
	page_table[VIDEO_PAGE_NUM+2].page_addr = (TERM2_VID >> FOUR_KB_SHIFT);
	page_table[VIDEO_PAGE_NUM+2].available = 0;		//we are not using these bits
	page_table[VIDEO_PAGE_NUM+2].global = ENABLE;
	page_table[VIDEO_PAGE_NUM+2].zero_pad = 0;
	page_table[VIDEO_PAGE_NUM+2].dirty = DISABLE;
	page_table[VIDEO_PAGE_NUM+2].accessed = DISABLE;
	page_table[VIDEO_PAGE_NUM+2].cache_dis = DISABLE;
	page_table[VIDEO_PAGE_NUM+2].write_thru = DISABLE;
	page_table[VIDEO_PAGE_NUM+2].user_super = SUPER;
	page_table[VIDEO_PAGE_NUM+2].read_write = ENABLE;
	page_table[VIDEO_PAGE_NUM+2].present = ENABLE;
	
	// make a 4kB page for the terminal 3 video memory save
	page_table[VIDEO_PAGE_NUM+3].page_addr = (TERM3_VID >> FOUR_KB_SHIFT);
	page_table[VIDEO_PAGE_NUM+3].available = 0;		//we are not using these bits
	page_table[VIDEO_PAGE_NUM+3].global = ENABLE;
	page_table[VIDEO_PAGE_NUM+3].zero_pad = 0;
	page_table[VIDEO_PAGE_NUM+3].dirty = DISABLE;
	page_table[VIDEO_PAGE_NUM+3].accessed = DISABLE;
	page_table[VIDEO_PAGE_NUM+3].cache_dis = DISABLE;
	page_table[VIDEO_PAGE_NUM+3].write_thru = DISABLE;
	page_table[VIDEO_PAGE_NUM+3].user_super = SUPER;
	page_table[VIDEO_PAGE_NUM+3].read_write = ENABLE;
	page_table[VIDEO_PAGE_NUM+3].present = ENABLE;
	
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
	
	// make a 4MB page in the directory that will point to the currently active process (index 32 because it is the 128MB region)
	page_dir[USER_PAGE_IDX].table_addr = (USER_START >> FOUR_KB_SHIFT);
	page_dir[USER_PAGE_IDX].available = 0;			// we are not using this
	page_dir[USER_PAGE_IDX].ignored = 0;
	page_dir[USER_PAGE_IDX].page_size = 1;			// this is a 4MB page
	page_dir[USER_PAGE_IDX].zero_pad = 0;
	page_dir[USER_PAGE_IDX].accessed = DISABLE;
	page_dir[USER_PAGE_IDX].cache_dis = DISABLE;
	page_dir[USER_PAGE_IDX].write_thru = DISABLE;
	page_dir[USER_PAGE_IDX].user_super = USER;
	page_dir[USER_PAGE_IDX].read_write = ENABLE;
	page_dir[USER_PAGE_IDX].present = ENABLE;
	
}
