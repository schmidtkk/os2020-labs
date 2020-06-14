#include "../../include/task.h"
#include "../../include/kmem.h"
#include "../../include/myPrintk.h"

extern unsigned long noPreemption;

typedef struct
{
	myTCB *task;
	unsigned long next;
}rdyQueueFCFSNode;

typedef struct
{
	rdyQueueFCFSNode *head;
	rdyQueueFCFSNode *tail;
}rdyQueueFCFS;

rdyQueueFCFS rqFCFS;

void rqFCFSInit(void)
{
	noPreemption = 1;
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
	return rqFCFS.head->task;
}

/* tskEnqueueFCFS: insert into the tail node */
void tskEnqueueFCFS(myTCB *tsk)
{
	rdyQueueFCFSNode *addNode = kmalloc(sizeof(rdyQueueFCFSNode));
	addNode->task = tsk;

	if (rqFCFSIsEmpty())
	{
		rqFCFS.head = rqFCFS.tail = addNode;
		addNode->next = 0;
	}
	else
	{
		rqFCFS.tail->next = addNode;
		rqFCFS.tail = addNode;
		addNode->next = 0;
	}
}

/* tskDequeueFCFS: delete the first node */
void tskDequeueFCFS()
{
	__asm__ __volatile__("call disable_interrupt");
	
	if (rqFCFSIsEmpty())
		myPrintk(0x7, "ohho, FCFS seems to have some mistake...\n");
	else if (rqFCFS.head->next == 0)
		rqFCFS.head = rqFCFS.tail = 0;
	else
		rqFCFS.head = rqFCFS.head->next;

	__asm__ __volatile__("call enable_interrupt");
}

void initFCFS(void){
	rqFCFSInit();
}


void scheduleFCFS(void)
{
	while (1)
	{
		if (!rqFCFSIsEmpty())
		{
			myPrintk(0xf, "FCFS Scheduling...\n");

			myTCB *nextTsk = nextFCFSTsk();
			idleTask->state = TSK_READY;
			nextTsk->state = TSK_RUNNING;
			//switch
			context_switch(idleTask, nextTsk);
			//return
			// myPrintk(0xf, "Return to IdleTask...\n");

			nextTsk->state = TSK_DONE;
			tskDequeueFCFS();
			destroyTsk(nextTsk->id);

			idleTask->state = TSK_RUNNING;
		}
	}
}

scheduler schedulerFCFS = {
	.nextTsk_func = nextFCFSTsk,
	.enqueueTsk_func = tskEnqueueFCFS,
	.dequeueTsk_func = tskDequeueFCFS,
	.schedulerInit_func = initFCFS,
	.schedule = scheduleFCFS,
	.createTsk_hook = 0,
	.tick_hook = 0
};