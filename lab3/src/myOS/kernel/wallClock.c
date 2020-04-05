
int hh=0, mm=0, ss=0;
extern void set_timer_hook(void (*func)(void));
extern int ticks;
extern int myPrintk(int color, const char* format, ...);
extern int putBottomRight(int color, const char* format, ...);

void maybeUpdateWallClock(void)
{
        if(ticks%100) return;
        ss=(ss+1)%60;
        if(!ss) mm=(mm+1)%60;
        if(!mm && !ss) hh=(hh+1)%24;
        putBottomRight(0x7, "%2d : %2d : %2d",hh,mm,ss);
}

void setWallClock(int h, int m, int s)
{
        if(h<0 || h>23) return;
        if(m<0 || m>59) return;
        if(s<0 || s>59) return;

        hh=h,mm=m,ss=s;
        set_timer_hook(maybeUpdateWallClock);
        putBottomRight(0x7, "%2d : %2d : %2d",hh,mm,ss);
}

void getWallClock(int *h, int *m, int *s)
{
        *h = hh;
        *m = mm;
        *s = ss;
}

