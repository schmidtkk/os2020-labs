#ifndef __TASK_H__
#define __TASK_H__

#ifndef USER_TASK_NUM
#include "../../userApp/userApp.h"
#endif


#define idleTask TCBPtr[0]

#define initTask TCBPtr[1]

#define TASK_NUM (2 + USER_TASK_NUM)   // at least: 0-idle, 1-init

#define initTskBody myMain         // connect initTask with myMain

void initTskBody(void);

void CTX_SW(void);

typedef struct myTCB {
    unsigned long *stkTop;    	/* 栈顶指针 */
	unsigned long *stkBase;		/* 栈空间最高地址 */
	unsigned long *stkLimit;	/* 栈空间最低地址 */
	unsigned long id;			/* 进程编号 */
	unsigned long state;		/* 进程状态 */
	unsigned long next;			/* 下一个TCB */
	void (*run)(void);			/* 入口函数 */
} myTCB;

myTCB * TCBPtr[TASK_NUM];

myTCB * currentTsk;             /* 当前任务 */

void TaskManagerInit(void);
int createTsk(void (*tskBody)(void));
void destroyTsk(int tskIndex);
void tskStart(myTCB *tsk);
void tskEnd(void);

#endif
