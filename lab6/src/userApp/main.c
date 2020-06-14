#include "shell.h"
#include "../myOS/include/task.h"
#include "../myOS/include/taskPara.h"

#include "../myOS/userInterface.h"

extern void startShell(void);

extern myTsk0(void);
extern myTsk1(void);
extern myTsk2(void);
extern disp(void);

void shellTask(void){
	myPrintf(0x4,"shellTask()\n");
	initShell();
    startShell();
	tskEnd();
}

void __main(void)
{
	setSysScheduler(SJF);
	TaskManagerInit();
}

void myMain(void){
	myPrintf(0x4,"myMain()\n");

	int shell = createTsk(shellTask, 0);
	int task0 = createTsk(myTsk0, 0);
	int task1 = createTsk(myTsk1, 0);
	int task2 = createTsk(myTsk2, 0);
	
	tskPara para[4];
	for(int i=0;i<4;i++) initTskPara(&para[i]);
	setTskPara(ARRTIME, 600, &para[0]);
	setTskPara(EXETIME, 1000, &para[0]);
	_setTskPara(TCBPtr[shell], &para[0]);

	setTskPara(ARRTIME, 500, &para[1]);
	setTskPara(EXETIME, 20, &para[1]);
	_setTskPara(TCBPtr[task0], &para[1]);

	setTskPara(ARRTIME, 300, &para[2]);
	setTskPara(EXETIME, 10, &para[2]);
	_setTskPara(TCBPtr[task1], &para[2]);

	setTskPara(ARRTIME, 500, &para[3]);
	setTskPara(EXETIME, 10, &para[3]);
	_setTskPara(TCBPtr[task2], &para[3]);

	enableTsk(shell);
	enableTsk(task0);
	enableTsk(task1);
	enableTsk(task2);


	tskEnd();
}
