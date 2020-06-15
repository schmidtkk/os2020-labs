#include "../include/task.h"
#include "../include/taskPara.h"
#include "../include/taskArr.h"
#include "../include/myPrintk.h"
#include "../include/kmem.h"
#include "../include/mem.h"
#include "../include/scheduler.h"
#include "../include/priorityQueue.h"

unsigned long tskCnt = 0;
unsigned long timer = 0;

/* createTsk
 * tskBody():
 * return value: taskIndex or, if failed, -1
 */

int createTsk(void (*tskBody)(void), tskPara *para)
{

	int flag = -1;
	int i;
	for (i = 0; i < TASK_NUM; i++)
	{
		if (TCBPtr[i] == 0)
		{
			TCBPtr[i] = (myTCB *)kmalloc(sizeof(myTCB));
			TCBPtr[i]->preempted = 0;
			TCBPtr[i]->id = i;
			TCBPtr[i]->run = tskBody;
			TCBPtr[i]->state = TSK_WAIT;
			TCBPtr[i]->stkLimit = (unsigned long *)kmalloc(STACK_SIZE);
			TCBPtr[i]->stkBase = TCBPtr[i]->stkLimit + STACK_SIZE - 1;
			TCBPtr[i]->stkTop = TCBPtr[i]->stkLimit + STACK_SIZE;

			if(!para)
			{
				_setTskPara(TCBPtr[i], &defaultTskPara);
			}
			else
			{
				_setTskPara(TCBPtr[i], para);
			}

			//init stack
			*--TCBPtr[i]->stkTop = (unsigned long)0x0202;	//eflags
			*--TCBPtr[i]->stkTop = (unsigned long)0x08;		//cs
			*--TCBPtr[i]->stkTop = (unsigned long)tskBody;	//eip
			*--TCBPtr[i]->stkTop = (unsigned long)0x0202;	//eflags

			*--TCBPtr[i]->stkTop = (unsigned long)0xAAAAAAAA; //eax
			*--TCBPtr[i]->stkTop = (unsigned long)0xCCCCCCCC; //ecx
			*--TCBPtr[i]->stkTop = (unsigned long)0xDDDDDDDD; //edx
			*--TCBPtr[i]->stkTop = (unsigned long)0xBBBBBBBB; //ebx

			*--TCBPtr[i]->stkTop = (unsigned long)0x44444444; //esp
			*--TCBPtr[i]->stkTop = (unsigned long)0x55555555; //ebp
			*--TCBPtr[i]->stkTop = (unsigned long)0x66666666; //esi
			*--TCBPtr[i]->stkTop = (unsigned long)0x77777777; //edi

			flag = i;
			tskCnt++;
			break;
		}
	}
	if(sysScheduler.createTsk_hook) sysScheduler.createTsk_hook(TCBPtr[i]);
	return flag;
}

int enableTsk(int tskIndex)
{
	__asm__ __volatile__("call disable_interrupt");

	if(TCBPtr[tskIndex]->arrtime==0) tskStart(TCBPtr[tskIndex]);
	else tskStartDelayed(TCBPtr[tskIndex]);

	__asm__ __volatile__("call enable_interrupt");

}
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


void context_switch(myTCB *prevTsk, myTCB *nextTsk)
{
	__asm__ __volatile__("call disable_interrupt");
	prevTSK_StackPtr = &(prevTsk->stkTop);
	nextTSK_StackPtr = nextTsk->stkTop;
	nextTSK = nextTsk;
	CTX_SW();
	__asm__ __volatile__("call enable_interrupt");
}
void context_switch_interrupt(myTCB *prevTsk, myTCB *nextTsk)
{
	__asm__ __volatile__("call disable_interrupt");
	prevTSK_StackPtr = &(prevTsk->stkTop);
	nextTSK_StackPtr = nextTsk->stkTop;
	nextTSK = nextTsk;
	nextTsk->preempted = 0;
	CTX_SW();
	__asm__ __volatile__("call enable_interrupt");
}

void schedule(void)
{
	sysScheduler.schedule();
}


void tskStart(myTCB *tsk){
	tsk->state = TSK_READY;
	sysScheduler.enqueueTsk_func(tsk);
}
void tskEnd(void){
	//context switch to idle task
	context_switch(currentTsk, idleTask);
}
void initScheduler(void){
	initArrList();
	sysScheduler.schedulerInit_func();
}


unsigned long BspContextBase[STACK_SIZE];
unsigned long *BspContext;

//start multitasking
void startMultitask(void)
{
	BspContext = BspContextBase + STACK_SIZE - 1;
	prevTSK_StackPtr = &BspContext;
	currentTsk = sysScheduler.nextTsk_func();
	sysScheduler.dequeueTsk_func();
	nextTSK_StackPtr = currentTsk->stkTop;
	currentTsk->state = TSK_RUNNING;
	CTX_SW();
}

void TaskManagerInit()
{
	currTSK = &currentTsk;

	for (int i = 0; i < TASK_NUM; i++)
	{
		TCBPtr[i] = 0;
	}

	// 创建 idle 任务
	createTsk(idleTskBody, 0);
	idleTask->exetime = 0;
	idleTaskStackPtr = &(idleTask->stkTop);

	// 创建 init 任务（使用 initTskBody）
	createTsk(initTskBody, 0);

	// 切入多任务状态
	initScheduler();
	tskStart(idleTask);
	tskStart(initTask);
	startMultitask(); 
}

void preemptionParser(void)
{
	timer++;
	if(sysScheduler.tick_hook) sysScheduler.tick_hook();
	tick_hook_arr();
	if(noPreemption) return;
	if (currentTsk!=idleTask && !(timer % timeSlice))
	{
		// myPrintk(0xf,"preempt task: %d\n",currentTsk);

		currentTsk->preempted = 1;
		if(idleTask->preempted)
		{
			context_switch_interrupt(currentTsk, idleTask);
		}
		else
		{
			context_switch(currentTsk, idleTask);
		}
		
	}
}

unsigned int getSysScheduler(void)
{
	return sched;
}
void setSysScheduler(unsigned int what)
{
	switch (what)
	{
	case FCFS:
		sched = FCFS;
		sysScheduler = schedulerFCFS;
		break;
	case RR:
		sched = RR;
		sysScheduler = schedulerRR;
		break;
	case SJF:
		sched = SJF;
		sysScheduler = schedulerSJF;
		break;
	default:
		sched = FCFS;
		sysScheduler = schedulerFCFS;
		break;
	}
}
void getSysSchedulerPara(unsigned int who, unsigned int *para)
{
	switch (who)
	{
	case NEXT_FUNC:
		*para = sysScheduler.nextTsk_func;
		break;
	case ENQ_FUNC:
		*para = sysScheduler.enqueueTsk_func;
		break;
	case DEQ_FUNC:
		*para = sysScheduler.dequeueTsk_func;
		break;
	case INIT_FUNC:
		*para = sysScheduler.schedulerInit_func;
		break;
	case SCHED_FUNC:
		*para = sysScheduler.schedule;
		break;
	case CREATE_FUNC:
		*para = sysScheduler.createTsk_hook;
		break;
	case TICK_FUNC:
		*para = sysScheduler.tick_hook;
		break;
	default:
		*para = -1;
		break;
	}
}
void setSysSchedulerPara(unsigned int who, unsigned int para)
{
	switch (who)
	{
	case NEXT_FUNC:
		sysScheduler.nextTsk_func = para;
		break;
	case ENQ_FUNC:
		sysScheduler.enqueueTsk_func = para;
		break;
	case DEQ_FUNC:
		sysScheduler.dequeueTsk_func = para;
		break;
	case INIT_FUNC:
		sysScheduler.schedulerInit_func = para;
		break;
	case SCHED_FUNC:
		sysScheduler.schedule = para;
		break;
	case CREATE_FUNC:
		sysScheduler.createTsk_hook = para;
		break;
	case TICK_FUNC:
		sysScheduler.tick_hook = para;
		break;
	}
}