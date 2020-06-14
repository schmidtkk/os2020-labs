#include "../../include/task.h"
#include "../../include/myPrintk.h"
#include "../../include/kmem.h"

extern unsigned long noPreemption;

typedef struct
{
	myTCB *task;
	unsigned long next;
	unsigned long prev;
} rdyQueueRRNode;
typedef struct rdyQueueRR
{
	rdyQueueRRNode *head;
	rdyQueueRRNode *tail;
	rdyQueueRRNode *current;
} rdyQueueRR;

rdyQueueRR rqRR;

void rqRRInit(void)
{
	noPreemption = 0;
	rqRR.current = 0;
	rqRR.head = rqRR.tail = 0;
}

int rqRRIsEmpty(void)
{
	if (rqRR.tail == 0)
		return 1;
	else
		return 0;
}

myTCB *nextRRTsk(void)
{
	return rqRR.current->task;
}

/* tskEnqueueRR: insert into the tail node */
void tskEnqueueRR(myTCB *tsk)
{
	rdyQueueRRNode *addNode = kmalloc(sizeof(rdyQueueRRNode));
	addNode->task = tsk;
	if (rqRRIsEmpty())
	{
		rqRR.current = addNode;
		rqRR.head = rqRR.tail = addNode;
		addNode->next = addNode;
		addNode->prev = addNode;
	}
	else
	{
		rqRR.tail->next = addNode;
		addNode->prev = rqRR.tail;
		rqRR.tail = addNode;
		addNode->next = rqRR.head;
		rqRR.head->prev = rqRR.tail;
	}
}

/* tskDequeueRR: delete the first node */
void tskDequeueRR()
{
	__asm__ __volatile__("call disable_interrupt");


	if (rqRRIsEmpty())
	{
		myPrintk(0x7, "ohho, RR seems to have some mistake...\n");
		while(1);
	}
	else
	{
		if(rqRR.head == rqRR.tail)
		{
			rqRRInit();
		}
		else
		{
			rdyQueueRRNode *prev = (rdyQueueRRNode *)rqRR.current->prev;
			rdyQueueRRNode *next = (rdyQueueRRNode *)rqRR.current->next;
			if(rqRR.current == rqRR.head)
			{
				rqRR.head = next;
				rqRR.tail->next = next;
				next->prev = rqRR.tail;
			}
			else if(rqRR.current == rqRR.tail)
			{
				rqRR.tail = prev;
				rqRR.head->prev = prev;
				prev->next = rqRR.head;
			}
			else
			{
				prev->next = next;
				next->prev = prev;
			}
			rqRR.current = next;
		}
	}

	__asm__ __volatile__("call enable_interrupt");

}

void initRR(void){
	rqRRInit();
}

void scheduleRR(void)
{
	while (1)
	{
		if (!rqRRIsEmpty())
		{
			// myPrintk(0xf, "RR Scheduling...\n");

			myTCB *nextTsk = nextRRTsk();

			idleTask->state = TSK_READY;
			nextTsk->state = TSK_RUNNING;
			//switch

			// myPrintk(0xf, "idle: %d...\n",idleTask);
			// myPrintk(0xf, "next: %d...\n",nextTsk);

			if(nextTsk->preempted)
			{
				context_switch_interrupt(idleTask, nextTsk);
			}
			else
			{
				context_switch(idleTask, nextTsk);
			}
			
			//return
			// myPrintk(0xf, "Return to IdleTask...\n");

			if (nextTsk->preempted)
			{
				nextTsk->state = TSK_READY;
				rqRR.current = rqRR.current->next;
			}
			else
			{
				nextTsk->state = TSK_DONE;
				tskDequeueRR();
				destroyTsk(nextTsk->id);
			}

			idleTask->state = TSK_RUNNING;
		}
	}
}

scheduler schedulerRR = {
	.nextTsk_func = nextRRTsk,
	.enqueueTsk_func = tskEnqueueRR,
	.dequeueTsk_func = tskDequeueRR,
	.schedulerInit_func = initRR,
	.schedule = scheduleRR,
	.createTsk_hook = 0,
	.tick_hook = 0
};