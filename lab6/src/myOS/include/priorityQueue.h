#include "task.h"

#ifndef __PRIORITY_QUEUE_H__
#define __PRIORITY_QUEUE_H__

#define QUEUE_SIZE 256

typedef struct
{
	myTCB *task;
}priorityQueueNode;

typedef struct
{
	priorityQueueNode *heap;
	int (*compareLT)(unsigned long *priorityQueueNode1, unsigned long *priorityQueueNode2);
	int total;
}priorityQueue;

void priorityQueueInit(priorityQueue *PQ, int (*cmpLT)(unsigned long *priorityQueueNode1, unsigned long *priorityQueueNode2));
int priorityQueuePush(priorityQueue *PQ, myTCB *task);
int priorityQueuePop(priorityQueue *PQ);
int priorityQueueIsEmpty(priorityQueue *PQ);
priorityQueueNode* priorityQueueTop(priorityQueue *PQ);
void priorityQueueDisplay(priorityQueue *PQ);

#endif