#include "shell.h"

extern void startShell(void);
void myMain(void){
	initShell();
    startShell();
    return;
}
