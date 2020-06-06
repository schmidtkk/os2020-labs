#include "include/i8253.h"
#include "include/i8259A.h"
#include "include/myPrintk.h"
#include "include/vga.h"
#include "include/wallClock.h"
#include "include/task.h"

// extern void test(void);
extern void myMain(void);

extern unsigned long pMemHandler;
extern void pMemInit(void);

void osStart(void)
{
    __asm__ __volatile__("Call enable_interrupt");
    init8253();
    init8259A();
	clear_screen();
	setWallClock(0,0,0);
	pMemInit();
	if(!pMemHandler){
		myPrintk(0x2,"FAIL TO INITIALIZE MEMORY, ShutDown......\n");
		while(1);
	}
	myPrintk(0x2,"START RUNNING......\n");
	TaskManagerInit();
	// test();
	myPrintk(0x2, "STOP RUNNING......ShutDown\n");
	while(1);
}
