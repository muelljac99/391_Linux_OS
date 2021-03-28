#include "tests.h"
#include "x86_desc.h"
#include "lib.h"
#include "irq_asm.h"
#include "paging.h"
#include "i8259.h"
#include "rtc.h"
#include "terminal.h"
#include "file_dir.h"

#define PASS 1
#define FAIL 0

/* format these macros as you see fit */
#define TEST_HEADER 	\
	printf("[TEST %s] Running %s at %s:%d\n", __FUNCTION__, __FUNCTION__, __FILE__, __LINE__)
#define TEST_OUTPUT(name, result)	\
	printf("[TEST %s] Result = %s\n", name, (result) ? "PASS" : "FAIL");

static inline void assertion_failure(){
	/* Use exception #15 for assertions, otherwise
	   reserved by Intel */
	asm volatile("int $15");
}


/* Checkpoint 1 tests */

/* IDT Test - Example
 * 
 * Asserts that first 10 IDT entries are not NULL
 * Inputs: None
 * Outputs: PASS/FAIL
 * Side Effects: None
 * Coverage: Load IDT, IDT definition
 * Files: x86_desc.h/S; set_idt.h/c; service_irq.h/c
 */
int idt_test(){
	TEST_HEADER;

	int i;
	int result = PASS;
	for (i = 0; i < 10; ++i){
		if ((idt[i].offset_15_00 == NULL) && 
			(idt[i].offset_31_16 == NULL)){
			assertion_failure();
			result = FAIL;
		}
	}

	return result;
}

/* System Call Test
 * 
 * Checks that forcing an interrupt to the irq 0x80 results in a pseudo system call
 * Inputs: None
 * Outputs: "SYSTEM CALL" if passed, anything else is fail
 * Side Effects: Prints to screen
 * Coverage: Proper IDT coresspondence with irq numbers
 * Files: x86_desc.h/S; set_idt.h/c; service_irq.h/c
 */
void sys_call_test(){
	TEST_HEADER;
	
	// force a system call interupt to check that the idt maps correctly
	asm volatile("int $0x80");
}

/* Undefined Interrupt Test
 * 
 * Checks that forcing an interrupt to a line with no interrupt handler and non present idt entry calls exception #B
 * Inputs: None
 * Outputs: "EXCEPTION #B" if passed, anything else is fail
 * Side Effects: Prints to screen
 * Coverage: Proper IDT coresspondence with irq numbers and idt present value
 * Files: x86_desc.h/S; set_idt.h/c; service_irq.h/c
 */
void undefined_int_test(){
	TEST_HEADER;
	
	//force a interrupt to an unintialized interrupt
	asm volatile("int $0x34");
}

/* Divide by Zero Test
 * 
 * Checks that dividing by zero generates an exception 0
 * Inputs: None
 * Outputs: "EXCEPTION #0" if passed, anything else is fail
 * Side Effects: Prints to screen and Freezes
 * Coverage: Proper IDT corespondence with irq numbers and exception handling function
 * Files: x86_desc.h/S; set_idt.h/c; service_irq.h/c
 */
void divide_by_0_test(){
	TEST_HEADER;
	
	//performs a divide by zero in order to confirm that this will lead to exception #0
	int x = 4;
	int y = 0;
	x = x/y;
}

/* Page Fault Test
 * 
 * Checks that accessing memory on a non present page creates a page fault or exception #E
 * Inputs: None
 * Outputs: "EXCEPTION #E" if passed, anything else is fail
 * Side Effects: Prints to screen and Freezes
 * Coverage: Proper IDT corespondence with irq numbers and exception handling function
 *			 Proper page set up and only make present the necessary pages
 * Files: x86_desc.h/S; set_idt.h/c; service_irq.h/c; paging.h/c
 */
void page_fault_test(){
	TEST_HEADER;
	
	//tries to access memory where a page has not been created
	int* x = NULL;
	int y;
	y = (*x);
}

/* Page Fault Test 2
 * 
 * Checks that accessing memory on a non present page creates a page fault or exception #E
 * Inputs: None
 * Outputs: "EXCEPTION #E" if passed, anything else is fail
 * Side Effects: Prints to screen and Freezes
 * Coverage: Proper IDT corespondence with irq numbers and exception handling function
 *			 Proper page set up and only make present the necessary pages
 * Files: x86_desc.h/S; set_idt.h/c; service_irq.h/c; paging.h/c
 */
void page_fault_test2(){
	TEST_HEADER;
	
	//tries to access memory where a page has not been created
	int* x = (int*)(0x00800000);	//the first addr outside of kernel page
	int y;
	y = (*x);
}

/* Defined Page Test
 * 
 * Checks that writing and reading from a defined page can be done and corresponds to the proper value
 * Inputs: None
 * Outputs: Test Pass if success and Test Fail if fail
 * Side Effects: Prints to screen and edits kernel memory
 * Coverage: Proper page set up and only make present the necessary pages
 * Files: paging.h/c
 */
int defined_page_test(){
	TEST_HEADER;
	int result = PASS;
	
	//sets a value in the defined page space and reads it to ensure no page faulting and accurate access
	int* x = (int*)(KERNEL_START + VIDEO_START); 			// an offset to a empty space in the kernel page
	int y;
	(*x) = -1;
	y = (*x);
	if(y != -1)
		result = FAIL;
	return result;
}

/* Garbage IRQ Test
 * 
 * Checks that performing any PIC instructions with an invalid IRQ will print an error but not crash
 * Inputs: None
 * Outputs: Print invalid IRQ statement three times if success, anything else fail
 * Side Effects: Prints to screen
 * Coverage: Proper handling if incorrect inputs for our PIC functions
 * Files: i8259.h/c
 */
void garbage_irq_test(){
	TEST_HEADER;
	
	// call our functions with irq numbers outside the range of acceptable ones
	enable_irq(0x05);
	disable_irq(0x30);
	send_eoi(0x71);
}

/* Checkpoint 2 tests */

void rtc_write_test(int freq){
	unsigned char r[4] = "RTC";
	
	rtc_open(r);
	rtc_write(1, &freq, 0);
	while(1){
		rtc_read(1, &freq, 0);
		printf("1");
	}
}

void terminal_echo_test(){
	unsigned char buffer[20];
	unsigned int size;
	while(1){
		size = terminal_read(0, buffer, 20);
		terminal_write(0, buffer, size);
	}
}

void file_read_test(){
	uint8_t filename[128];
	uint8_t term_name[9] = "terminal";
	uint8_t buffer[8192];
	uint32_t read_len, name_len;
	terminal_open(term_name);
	name_len = terminal_read(0, filename, 33);
	terminal_write(0, filename, name_len);
	file_open(filename);
	read_len = file_read(1, buffer, 100);
	file_close(0);
	printf("Bytes Read: %d\n", read_len);
	terminal_write(0, buffer, read_len);
}

void dir_read_test(){
	uint8_t buffer[8192];
	int i;
	uint8_t dir[2] = ".";
	uint8_t term[9] = "terminal";
	dir_open(dir);
	terminal_open(term);
	for(i=0; i<(*file_sys_addr); i++){
		dir_read(0, buffer, 80);
		terminal_write(0, buffer, 80);
	}
}

/* Checkpoint 3 tests */
/* Checkpoint 4 tests */
/* Checkpoint 5 tests */


/* Test suite entry point */
void launch_tests(){
	TEST_OUTPUT("idt_test", idt_test());
	/* checkpoint 1 tests */
	//sys_call_test();
	//undefined_int_test();
	//divide_by_0_test();
	//page_fault_test();
	//page_fault_test2();
	//TEST_OUTPUT("defined_page_test", defined_page_test());
	//garbage_irq_test();
	
	/* checkpoint 2 tests */
	//rtc_write_test(512);
	//terminal_echo_test();
	dir_read_test();
	file_read_test();
}
