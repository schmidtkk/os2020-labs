#include "../include/io.h"

void init8253(void){
    /* 1,193,180 Hz to 8253 */
    unsigned short fq = 11932;//100Hz
    unsigned char high,low;

    high = fq>>8;
    low = fq;
    outb(0x43,0x34);//set control byte
    io_wait();
    outb(0x40,low);//set high 8 bits
    io_wait();
    outb(0x40,high);// set low 8 bits
    io_wait();
}


