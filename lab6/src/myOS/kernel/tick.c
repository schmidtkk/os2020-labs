#include "../include/kmem.h"

#define SIZE 256
int ticks=0;
typedef void(*fun)(void); // typedef hook function
fun *list;// hook function list
int cnt=0;// hook counter

void tickInit(void)
{
	list = kmalloc(sizeof(fun)*SIZE);
}
// add hook function to function list
void set_timer_hook(void (*func)(void)){
    list[cnt++]=func;
}

// call all hook functions
void timer_hook_parse(void){
    for(int i=0;i<cnt;i++) list[i]();
}

// called by time interrupt handler
void tick(void){
    ticks++;
    timer_hook_parse();
}

unsigned long getTick(void)
{
	return ticks;
}

//n second
void busy_n_second(int n){
	int second = 0x3000000;// 1秒内大约多少条乘法指令 毛估估
	int a=21,b=31,c;
	for(int i=0;i<n;i++)
		for(int j=0;j<second;j++) c=a*b;  //about 1sec
}

//n ms
void busy_n_ms(int n){
	int second = 0x8000;// 1秒内大约多少条乘法指令 毛估估
	int a=21,b=31,c;
	for(int i=0;i<n;i++)
		for(int j=0;j<second;j++) c=a*b;  //about 1m
}
