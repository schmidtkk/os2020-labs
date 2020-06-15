#include "../include/task.h"
#include "../include/priorityQueue.h"
#include "../include/myPrintk.h"


/* for task arriving */
extern void tskStart(myTCB *tsk);
extern unsigned long getTick(void);

/* time unit: tick */
/* zero arriving time: x ticks*/
unsigned int arrTimeBase = 0x0;

priorityQueue PQArr;

int cmpLTArr(priorityQueueNode *node1, priorityQueueNode *node2){
	if(node1->task->arrtime <= node2->task->arrtime) return 1;
	else return 0;
}
void initArrList(void){
	priorityQueueInit(&PQArr, cmpLTArr);
}

/* arrTime: small --> big */
void ArrListEnqueue(myTCB* tsk){   
	tsk->state = TSK_WAIT; 
    priorityQueuePush(&PQArr, tsk);
}

void tskStartDelayed(myTCB* tsk){
	ArrListEnqueue(tsk);
}

void tick_hook_arr(void){
	while(	!priorityQueueIsEmpty(&PQArr) &&
			priorityQueueTop(&PQArr)->task->arrtime + arrTimeBase <= getTick())
	{
		// myPrintk(0x7,"tick: %d\n", getTick());
		// priorityQueueDisplay(&PQArr);
		tskStart(priorityQueueTop(&PQArr)->task);
		priorityQueuePop(&PQArr);
	}	     
}
