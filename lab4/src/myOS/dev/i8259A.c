#include "../include/io.h"
#include "../include/myPrintk.h"


void init8259A(void){

    /* master chip ICW1~ICW4 */
    outb(0x20, 0x11);
    io_wait();
    outb(0x21, 0x20);
    io_wait();
    outb(0x21, 0x04);
    io_wait();
    outb(0x21, 0x03);
    io_wait();

    /* slave chip ICW1~ICW4 */
    outb(0xA0, 0x11);
    io_wait();
    outb(0xA1, 0x28);
    io_wait();
    outb(0xA1, 0x02);
    io_wait();
    outb(0xA1, 0x01);
    io_wait();


    /* set mask to enable time interrupt */
    char value = inb(0x21);
    outb(0x21, value &= ~1);

}
