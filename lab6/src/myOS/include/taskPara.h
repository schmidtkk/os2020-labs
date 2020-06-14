#include "task.h"

#ifndef __TASK_PARA_H__
#define __TASK_PARA_H__


#define MAX_PRIORITY_NUM 5
#define MAX_EXETIME 100

//option for setTskPara()/getTskPara
#define PRIORITY	 1
#define EXETIME		 2
#define ARRTIME		 3


extern tskPara defaultTskPara;
tskPara initTsk_para;

void _setTskPara(myTCB *task, tskPara *para);

void initTskPara(tskPara *buffer);
void setTskPara(unsigned int option, unsigned int value, tskPara *buffer);
void getTskPara(unsigned option, unsigned int *para, tskPara *buffer);
#endif