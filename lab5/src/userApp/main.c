#include "shell.h"
#include "../myOS/userInterface.h"

extern void startShell(void);

extern myTsk0(void);
extern myTsk1(void);
extern myTsk2(void);

void shellTask(void){
	myPrintf(0x4,"shellTask()\n");
	initShell();
    startShell();
	tskEnd();
}

void myMain(void){
	myPrintf(0x4,"myMain()\n");

	myTCB *shell = TCBPtr[createTsk(shellTask)];
	myTCB *task0 = TCBPtr[createTsk(myTsk0)];
	myTCB *task1 = TCBPtr[createTsk(myTsk1)];
	myTCB *task2 = TCBPtr[createTsk(myTsk2)];
	tskStart(shell);
	tskStart(task0);
	tskStart(task1);
	tskStart(task2);
	tskEnd();
}
