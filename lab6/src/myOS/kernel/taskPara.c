#include "../include/task.h"
#include "../include/taskPara.h"

extern void initLeftExeTime_sjf(myTCB* tsk);

// may modified during scheduler_init
tskPara defaultTskPara = {
    .priority = MAX_PRIORITY_NUM,
    .exetime = MAX_EXETIME,
    .arrtime = 0,
}; //task设计调度的一些参数的默认值

void _setTskPara(myTCB *task, tskPara *para){
	task->priority = para->priority;
	task->exetime = para->exetime;
	task->arrtime = para->arrtime;
}
void initTskPara(tskPara *buffer)
{
	*buffer = defaultTskPara;
}
void setTskPara(unsigned int option, unsigned int value, tskPara *buffer){

	switch (option)
	{
	case PRIORITY:
		buffer->priority = value;
		break;
	case EXETIME:
		buffer->exetime = value;
		break;
	case ARRTIME:
		buffer->arrtime = value;
		break;
	}
}

void getTskPara(unsigned option, unsigned int *para, tskPara *buffer){
	switch (option)
	{
	case PRIORITY:
		*para = buffer->priority;
		break;
	case EXETIME:
		*para = buffer->exetime;
		break;
	case ARRTIME:
		*para = buffer->arrtime;
		break;
	default:
		*para = -1;
		break;
	}
}
