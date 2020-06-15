#ifndef __TASK_H__
#define __TASK_H__

typedef struct{
    unsigned long *stkTop;    	/* 栈顶指针 */
	unsigned long *stkBase;		/* 栈空间最高地址 */
	unsigned long *stkLimit;	/* 栈空间最低地址 */
	unsigned long id;			/* 进程编号 */
	unsigned long state;		/* 进程状态 */
	unsigned long preempted;	/* 是否被抢占 */
	unsigned long priority;		/* 优先级(用于优先级调度) */
	unsigned long exetime;		/* 预计执行时间(tick) */
	unsigned long arrtime;		/* 到达时间(tick) */
	void (*run)(void);			/* 入口函数 */
} myTCB;

typedef struct{
	myTCB* (*nextTsk_func)(void);
	void (*enqueueTsk_func)(myTCB *tsk);
	void (*dequeueTsk_func)();
	void (*schedulerInit_func)(void);
	void (*schedule)(void);
	void (*createTsk_hook)(myTCB* created);
	void (*tick_hook)(void);
}scheduler;

#define NEXT_FUNC 0
#define ENQ_FUNC 1
#define DEQ_FUNC 2
#define INIT_FUNC 3
#define SCHED_FUNC 4
#define CREATE_FUNC 5
#define TICK_FUNC 6

#define FCFS 0
#define RR 1
#define SJF 2

#define timeSlice 5


typedef struct tskPara{
	unsigned int priority;
	unsigned int exetime;
	unsigned int arrtime;
} tskPara;

#ifndef USER_TASK_NUM
#include "../../userApp/userApp.h"
#endif
#define TASK_NUM (2 + USER_TASK_NUM)   	// at least: 0-idle, 1-init

#define idleTask TCBPtr[0]
#define initTask TCBPtr[1]
#define initTskBody myMain         // connect initTask with myMain

#define TSK_WAIT 0
#define TSK_READY 1
#define TSK_RUNNING 2
#define TSK_DONE 3
#define idleTskBody schedule

#define STACK_SIZE 4096 * 100				// 32k

void initTskBody(void);
void CTX_SW(void);
void context_switch(myTCB *prevTsk, myTCB *nextTsk);
void context_switch_interrupt(myTCB *prevTsk, myTCB *nextTsk);
void TaskManagerInit(void);
int createTsk(void (*tskBody)(void), tskPara *para);
void destroyTsk(int tskIndex);
void tskStart(myTCB *tsk);
void tskEnd(void);
unsigned int getSysScheduler(void);
void setSysScheduler(unsigned int what);
void getSysSchedulerPara(unsigned int who, unsigned int *para);
void setSysSchedulerPara(unsigned int who, unsigned int para);

unsigned long **prevTSK_StackPtr;
unsigned long *nextTSK_StackPtr;
unsigned long *idleTaskStackPtr;
unsigned long *currTSK;
unsigned long nextTSK;
unsigned long noPreemption;
unsigned long sched;

myTCB* TCBPtr[TASK_NUM];
myTCB* currentTsk;
scheduler sysScheduler;

#endif
