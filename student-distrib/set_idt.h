
#ifndef _SET_IDT_H
#define _SET_IDT_H

/* IDT Constants */
#define BASE_INT		32
#define SYS_CALL_IRQ	80

/* the function to fill the idt entries at boot */
void idt_fill(void);



#endif /* _SET_IDT_H */
