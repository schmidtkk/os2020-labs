// extern int myPrintf(int color,const char *format, ...);
// extern int myPrintk(int color,const char *format, ...);
// extern int uartPrintf(const char *format, ...);
// extern void clear_screen(void);
// extern void put_chars(char* str, int color, int row, int col);
extern void startShell(void);
void myMain(void){
    startShell();
    return;
}
