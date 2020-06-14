#include "task.h"

#ifndef __SCHEDULER_SJF_H__
#define __SCHEDULER_SJF_H__

void initSJF(void);
void scheduleSJF(void);
void tskDequeueSJF(void);
myTCB* nextSJFTsk(void);

scheduler schedulerSJF;

#endif