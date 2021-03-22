
#ifndef _PAGING_H
#define _PAGING_H

/* paging location info */
#define VIDEO_START 		0x000B8000
#define KERNEL_START 		0x00400000
#define FOUR_KB_SHIFT		12
#define VIDEO_PAGE_NUM		184					// this corresponds to the start of video memory divided by 4096 for the page size

/* page entry info bits */
#define USER 		1
#define SUPER 		0

#define ENABLE 		1
#define DISABLE 	0

/* load the starting entries into the directory and first table */
void load_page(void);

#endif /* _PAGING_H */
