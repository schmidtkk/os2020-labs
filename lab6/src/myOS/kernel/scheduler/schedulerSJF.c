#include "../../include/task.h"
#include "../../include/priorityQueue.h"
#include "../../include/myPrintk.h"

priorityQueue rqSJF;

int cmpLTSJF(priorityQueueNode *priorityQueueNode1, priorityQueueNode *priorityQueueNode2)
{
	if(priorityQueueNode1->task->exetime <= priorityQueueNode2->task->exetime) return 1;
	else return 0;
}

void rqSJFInit(void)
{
	noPreemption = 0;
	priorityQueueInit(&rqSJF, cmpLTSJF);
}

myTCB *nextSJFTsk(void)
{
	return priorityQueueTop(&rqSJF)->task;
}

int rqSJFIsEmpty(void){
	return priorityQueueIsEmpty(&rqSJF);
}

/* tskEnqueueSJF: insert into the tail node */
void tskEnqueueSJF(myTCB *tsk)
{
	priorityQueuePush(&rqSJF, tsk);
}

/* tskDequeueSJF: delete the first node */
void tskDequeueSJF()
{
	__asm__ __volatile__("call disable_interrupt");

	if (rqSJFIsEmpty())
	{
		myPrintk(0x7, "ohho, SJF seems to have some mistake...\n");
		while(1);
	}
	else
	{
		priorityQueuePop(&rqSJF);
	}

	__asm__ __volatile__("call disable_interrupt");

}

void initSJF(void){
	rqSJFInit();
}

void scheduleSJF(void)
{
	while (1)
	{
		if (!rqSJFIsEmpty())
		{
			// myPrintk(0xf, "SJF Scheduling...\n");

			// priorityQueueDisplay(&rqSJF);
			myTCB *nextTsk = nextSJFTsk();
			tskDequeueSJF();


			idleTask->state = TSK_READY;
			nextTsk->state = TSK_RUNNING;

			//switch

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
				if(nextTsk->exetime > timeSlice)
				{
					nextTsk->state = TSK_READY;
					nextTsk->exetime = nextTsk->exetime - timeSlice;
					tskEnqueueSJF(nextTsk);
				}
				else
				{
					myPrintk(0x4, "Time exceeded, terminating...\n");
					nextTsk->state = TSK_DONE;
					destroyTsk(nextTsk->id);
				}

				
			}
			else
			{
				nextTsk->state = TSK_DONE;
				destroyTsk(nextTsk->id);
			}


			idleTask->state = TSK_RUNNING;
		}
	}
}
scheduler schedulerSJF = {
	.nextTsk_func = nextSJFTsk,
	.enqueueTsk_func = tskEnqueueSJF,
	.dequeueTsk_func = tskDequeueSJF,
	.schedulerInit_func = initSJF,
	.schedule = scheduleSJF,
	.createTsk_hook = 0,
	.tick_hook = 0
};
