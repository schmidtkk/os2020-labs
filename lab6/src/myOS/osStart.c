#include "include/i8253.h"
#include "include/i8259A.h"
#include "include/myPrintk.h"
#include "include/vga.h"
#include "include/wallClock.h"
#include "include/task.h"
#include "include/kmem.h"


// extern void test(void);
extern void myMain(void);
extern unsigned long pMemHandler;
extern void pMemInit(void);
extern void tickInit(void);
extern void __main(void);

unsigned long TIME_INTERRUPT_STACK;

void osStart(void)
{
	clear_screen();
	
	pMemInit();
	if(!pMemHandler)
	{
		myPrintk(0x2,"FAIL TO INITIALIZE MEMORY, ShutDown......\n");
		while(1);
	}

	noPreemption = 1;
    init8253();
    init8259A();
	clear_screen();
	tickInit();
	setWallClock(0,0,0);
	

	TIME_INTERRUPT_STACK = kmalloc(STACK_SIZE);
	if(TIME_INTERRUPT_STACK == 0)
	{
		myPrintk(0x2,"FAIL TO ALLOCATE STACK FOR TIME, ShutDown......\n");
		while(1);
	}

    __asm__ __volatile__("Call enable_interrupt");
	myPrintk(0x2,"START RUNNING......\n");
	__main();
	myPrintk(0x2, "STOP RUNNING......ShutDown\n");
	while(1);
}
