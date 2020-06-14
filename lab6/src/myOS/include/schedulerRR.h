#include "task.h"

#ifndef __SCHEDULER_RR_H__
#define __SCHEDULER_RR_H__

void initRR(void);
void scheduleRR(void);
void tskDequeueRR(void);
myTCB* nextRRTsk(void);
void display(void);

scheduler schedulerRR;

#endif