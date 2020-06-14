#include "../include/priorityQueue.h"
#include "../include/kmem.h"
#include "../include/myPrintk.h"

void priorityQueueInit(priorityQueue *PQ, int (*cmpLT)(unsigned long *priorityQueueNode1, unsigned long *priorityQueueNode2))
{
	PQ->heap = kmalloc(sizeof(priorityQueueNode) * QUEUE_SIZE);
	PQ->compareLT = cmpLT;
	PQ->total = 0;
	for(int i=0;i<QUEUE_SIZE;i++)
	{
		(*(PQ->heap+i)).task = 0;
	}
}

int priorityQueuePush(priorityQueue *PQ, myTCB *task)
{
	//heap full, return 0
	if(PQ->total == QUEUE_SIZE - 1) return 0;

	int p = ++PQ->total;

	// myPrintk(0xf, "p: %d\n",p);

	(*(PQ->heap+p)).task = task;

	//go up
	priorityQueueNode *parent,*current;
	priorityQueueNode swap;
	while(p!=1){
		parent = PQ->heap + p/2;
		current = PQ->heap + p;
		if(PQ->compareLT(current,parent))
		{
			swap = *parent;
			*parent  = *current;
			*current = swap;
		}
		else
		{
			break;
		}

		p = p / 2;
	}

	//push success
	//priorityQueueDisplay(PQ);

	return 1;
}

int priorityQueuePop(priorityQueue *PQ)
{
	//heap empty, return 0;
	if(PQ->total == 0) return 0;

	if(PQ->total == 1)
	{
		//reset root
		(*(PQ->heap+1)).task = 0;
		//update total
		PQ->total--;
		//done
		return 1;
	}
	else
	{
		//root <- last
		*(PQ->heap+1)= *(PQ->heap + PQ->total);
		//reset last
		(*(PQ->heap + PQ->total)).task = 0;
		//update total
		PQ->total--;
	}
	

	//go down
	int p = 1;
	priorityQueueNode *current,*left,*right;
	priorityQueueNode swap;
	while(1){
		current = PQ->heap + p;
		left = PQ->heap + p*2;
		right = PQ->heap + p*2 +1;
		if(left->task && !right->task && PQ->compareLT(left,current))
		{
			swap = *current;
			*current = *left;
			*left = swap;
			p = p * 2;
		}
		else if(right->task && !left->task && PQ->compareLT(right,current))
		{
			swap = *current;
			*current = *right;
			*right = swap;
			p = p*2 + 1;
		}
		else if(left->task && right->task && PQ->compareLT(left,current) && PQ->compareLT(left,right))
		{
			swap = *current;
			*current = *left;
			*left = swap;
			p = p * 2;
		}
		else if(left->task && right->task && PQ->compareLT(right,current) && PQ->compareLT(right,left))
		{
			swap = *current;
			*current = *right;
			*right = swap;
			p = p*2 + 1;
		}
		else
		{
			break;
		}
	}

	//pop success
	return 1;
}
priorityQueueNode* priorityQueueTop(priorityQueue *PQ)
{
	return PQ->heap+1;
}

int priorityQueueIsEmpty(priorityQueue *PQ)
{
	if(PQ->total == 0) return 1;
	else return 0;
}

void priorityQueueDisplay(priorityQueue *PQ)
{
	myPrintk(0xf, "\npriorityQueueDisplay\n");
	for(int i=1;i<=PQ->total;i++)
	{
		myPrintk(0x4,"priorityQueueNode %d: %d, %d\n",i,(*(PQ->heap+i)).task->exetime, (*(PQ->heap+i)).task->arrtime);
	}
	myPrintk(0xf, "priorityQueueDisplay Finshed\n");
}

int cmpLT(priorityQueueNode *priorityQueueNode1, priorityQueueNode *priorityQueueNode2)
{
	if(priorityQueueNode1->task->exetime < priorityQueueNode2->task->exetime) return 1;
	else return 0;
}

// void debug(void)
// {
// 	priorityQueue *PQ;
// 	priorityQueueInit(PQ, cmpLT);
// 	for(int i=0;i<200;i++)
// 	{
// 		myTCB *tmp = kmalloc(sizeof(myTCB));
// 		if(tmp==0){
// 			myPrintk(0x4, "Fail to Alloc\n");
// 			return;
// 		}
// 		tmp->exetime = 999 - i;
// 		priorityQueuePush(PQ,tmp);
// 		myPrintk(0x7,"top: %d\n",priorityQueueTop(PQ)->task->exetime);
// 		myPrintk(0x7,"total: %d\n",PQ->total);
// 	}

// 	priorityQueueDisplay(PQ);
	
// 	for(int i=0;i<200;i++)
// 	{
// 		priorityQueuePop(PQ);
// 		if(priorityQueueTop(PQ)->task) myPrintk(0x7,"top: %d\n",priorityQueueTop(PQ)->task->exetime);
// 		else myPrintk(0x7,"top: N/A\n");
// 	}
	
// 	myPrintk(0xf,"DONE\n");
// }