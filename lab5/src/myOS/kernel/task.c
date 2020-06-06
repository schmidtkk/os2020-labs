#include "../include/task.h"
#include "../include/myPrintk.h"
#include "../include/kmem.h"
#include "../include/mem.h"

void schedule(void);
void destroyTsk(int takIndex);

#define STACK_SIZE 4096 * 8	 // 32k
#define CONTEXT_INFO_SIZE 34 // esp + pusha + pushf

#define TSK_WAIT 0
#define TSK_READY 1
#define TSK_RUNNING 2
#define TSK_DONE 3

unsigned long tskCnt = 0;

typedef struct rdyQueueFCFS
{
	myTCB *head;
	myTCB *tail;
	myTCB *idleTsk;
} rdyQueueFCFS;

rdyQueueFCFS rqFCFS;

void rqFCFSInit(myTCB *idleTsk)
{

	rqFCFS.idleTsk = idleTsk;
	rqFCFS.head = rqFCFS.tail = 0;
}

int rqFCFSIsEmpty(void)
{
	if (rqFCFS.tail == 0)
		return 1;
	else
		return 0;
}

myTCB *nextFCFSTsk(void)
{
	return rqFCFS.head;
}

/* tskEnqueueFCFS: insert into the tail node */
void tskEnqueueFCFS(myTCB *tsk)
{
	if (rqFCFS.head == 0)
	{
		rqFCFS.head = rqFCFS.tail = tsk;
		tsk->next = 0;
	}
	else
	{
		rqFCFS.tail->next = tsk;
		rqFCFS.tail = tsk;
		tsk->next = 0;
	}
}

/* tskDequeueFCFS: delete the first node */
void tskDequeueFCFS()
{
	if (rqFCFS.head == 0)
		myPrintk(0x7, "ohho, FCFS seems to have some mistake...\n");
	else if (rqFCFS.head->next == 0)
		rqFCFS.head = rqFCFS.tail = 0;
	else
		rqFCFS.head = rqFCFS.head->next;
}

void tskStart(myTCB *tsk)
{
	tsk->state = TSK_READY;
	tskEnqueueFCFS(tsk);
}

void tskEnd(void)
{
	//context switch to idle task
	context_switch_task(currentTsk,idleTask);
}

/* createTsk
 * tskBody():
 * return value: taskIndex or, if failed, -1
 */

int createTsk(void (*tskBody)(void))
{

	int flag = -1;
	int i;
	for (i = 0; i < TASK_NUM; i++)
	{
		if (TCBPtr[i] == 0)
		{
			TCBPtr[i] = (myTCB *)kmalloc(sizeof(myTCB));
			TCBPtr[i]->id = i;
			TCBPtr[i]->run = tskBody;
			TCBPtr[i]->state = TSK_WAIT;
			TCBPtr[i]->stkLimit = (unsigned long *)kmalloc(STACK_SIZE);
			TCBPtr[i]->stkBase = TCBPtr[i]->stkLimit + STACK_SIZE - 1;
			TCBPtr[i]->stkTop = TCBPtr[i]->stkBase - 7;
			TCBPtr[i]->next = 0;
			*(TCBPtr[i]->stkBase - 7) = tskBody;

			//init stack
			unsigned long esp;
			__asm__ __volatile__ ("movl %%esp,%0":"=a"(esp));
			__asm__ __volatile__ ("movl %0,%%esp"::"a"(TCBPtr[i]->stkTop));
			__asm__ __volatile__ ("pushf");
			__asm__ __volatile__ ("pusha");
			__asm__ __volatile__ ("movl %%esp,(%0)"::"a"(&(TCBPtr[i]->stkTop)));
			__asm__ __volatile__ ("movl %0,%%esp"::"a"(esp));

			flag = i;
			tskCnt++;
			break;
		}
	}
	return flag;
}

/* destroyTsk
 * takIndex:
 * return value: void
 */
void destroyTsk(int tskIndex)
{
	if (tskCnt > 0 && tskIndex < TASK_NUM && TCBPtr[tskIndex] != 0)
	{
		kfree(TCBPtr[tskIndex]->stkLimit); //free stack
		kfree(TCBPtr[tskIndex]);		   //free TCB
		TCBPtr[tskIndex] = 0;			   //reset TCB pointer
		tskCnt--;
	}
}

unsigned long **prevTSK_StackPtr;
unsigned long *nextTSK_StackPtr;

void context_switch_stkptr(myTCB *prevTskStkPtr, myTCB *nextTskStkPtr)
{
	prevTSK_StackPtr = (unsigned long **)&prevTskStkPtr;
	nextTSK_StackPtr = (unsigned long *)nextTskStkPtr;
	CTX_SW();
}

void context_switch_task(myTCB *prevTsk, myTCB *nextTsk)
{
	prevTSK_StackPtr = &(prevTsk->stkTop);
	nextTSK_StackPtr = nextTsk->stkTop;
	CTX_SW();
}

void scheduleFCFS(void)
{
	while (1)
	{
		if (!rqFCFSIsEmpty())
		{
			myPrintk(0xf,"IdleTask Scheduling...\n");
			
			myTCB *nextTsk = nextFCFSTsk();
			idleTask->state = TSK_READY;
			nextTsk->state = TSK_RUNNING;
			currentTsk = nextTsk;
			//switch
			context_switch_task(idleTask, nextTsk);
			//return
			myPrintk(0xf,"Return to IdleTask...\n");
			tskDequeueFCFS();
			destroyTsk(currentTsk->id);

			idleTask->state = TSK_RUNNING;
			nextTsk->state = TSK_RUNNING;
			currentTsk = idleTask;
			
		}
	}
}

void schedule(void)
{
	scheduleFCFS();
}

/**
 * idle 任务
 */
void idleTskBody(void)
{
	schedule();
}

unsigned long BspContextBase[STACK_SIZE];
unsigned long *BspContext;

//start multitasking
void startMultitask(void)
{
	BspContext = BspContextBase + STACK_SIZE - 1;
	prevTSK_StackPtr = &BspContext;
	currentTsk = nextFCFSTsk();
	nextTSK_StackPtr = currentTsk->stkTop;
	currentTsk->state = TSK_RUNNING;
	tskDequeueFCFS();
	CTX_SW();
}

void TaskManagerInit(void)
{

	for (int i = 0; i < TASK_NUM; i++)
	{
		TCBPtr[i] = 0;
	}

	// 创建 idle 任务
	createTsk(idleTskBody);

	// 创建 init 任务（使用 initTskBody）
	createTsk(initTskBody);
	
	// 切入多任务状态
	tskStart(idleTask);
	tskStart(initTask);
	startMultitask();
}
