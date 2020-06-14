#define WHITE 0x7
extern void tskEnd(void);
extern int createTsk(void (*tskBody)(void));
extern int myPrintf(int color,const char *format, ...);
extern void busy_n_second(int n);
extern void busy_n_ms(int n);

void myTsk0(void){
	int j=1;	
	while(j<=10){		
		myPrintf(0x7,"myTSK0::%d    \n",j);
		busy_n_ms(120);
		j++;
	}	
	tskEnd();   //the task is end
}

void myTsk1(void){
	int j=1;	
	while(j<=10){
		myPrintf(0x7,"myTSK1::%d    \n",j);		
		busy_n_ms(120);
		j++;
	}	
	tskEnd();   //the task is end
}

void myTsk2(void){
	int j=1;	
	while(j<=10){
		myPrintf(0x7,"myTSK2::%d    \n",j);		
		busy_n_ms(120);
		j++;
	}	
	tskEnd();   //the task is end
}
