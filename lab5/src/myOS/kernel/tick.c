#define SIZE 256
int ticks=0;
typedef void(*fun)(void); // typedef hook function
fun list[SIZE];// hook function list
int cnt=0;// hook counter

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

