#include "task.h"

#ifndef __SCHEDULER_FCFS_H__
#define __SCHEDULER_FCFS_H__

void tskStartFCFS(myTCB *tsk);
void tskEndFCFS(void);
void initFCFS(void);
void scheduleFCFS(void);
void tskDequeueFCFS(void);
myTCB* nextFCFSTsk(void);

scheduler schedulerFCFS;
#endif