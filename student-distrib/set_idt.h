
#ifndef SET_IDT
#define SET_IDT

/* IDT Constants */
#define BASE_INT		32
#define SYS_CALL_IRQ	80

/* the function to fill the idt entries at boot */
void idt_fill(void);



#endif SET_IDT /* SET_IDT */