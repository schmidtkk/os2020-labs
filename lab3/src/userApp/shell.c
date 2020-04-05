#define NULL 0
#define MAXSIZE 256
#define MAXARGS 16
#define MAXARGLEN 32
extern int myPrintk(int color,const char *format, ...);
extern unsigned char uart_get_char(void);
extern void show_input(int color, char* str);
extern int uartPrintf(const char* format, ...);
extern void clear_screen(void);


// 命令处理函数
int cmd_handler(int, char **);
int help_handler(int, char **);
int cls_handler(int, char **);

// 帮助处理函数
void help_help(void);


struct command {
	char *cmd;
	int (*func)(int argc, char *argv[]);
	void (*help_func)(void);
	char *desc;
} cmds[] = {
	{"cmd", cmd_handler, NULL, "list all commands"},
	{"help", help_handler, help_help, "help [cmd]"},
    {"cls",cls_handler, NULL, "clear screen"},
	{"", NULL, NULL, ""}
};

// get length of a string
int strlen(char* str){
    int i;
    for(i=0;str[i]!=0;i++);
    return i;
}

// return whether str1 equals str2 (1=equal,2=not)
int streq(char* str1, char* str2){
    int len1,len2;
    len1=strlen(str1);
    len2=strlen(str2);
    if(len1!=len2) return 0;
    for(int i=0;i<len1;i++){
        if(str1[i]!=str2[i]) return 0;
    }
    return 1;
}

struct param{
    int argc;
    char args[MAXARGS][MAXARGLEN];
    char *argv[MAXARGS];
}parameters;


// parse raw command and extract parameters
void parseCmdStr(char* str){

    int p=0;
    int pp=0;
    int len=strlen(str);
    parameters.argc=0;
    while(p<len){
        if(str[p]==' '){
            parameters.argv[parameters.argc]=parameters.args[parameters.argc];
            parameters.args[parameters.argc++][pp]=0;
            p++;
            pp=0;
            continue;
        }
        
        parameters.args[parameters.argc][pp++]=str[p++];

    }
    parameters.argv[parameters.argc]=parameters.args[parameters.argc];
    parameters.args[parameters.argc++][pp]=0;

}
// help 的帮助
void help_help(void)
{
	myPrintk(0x7,"%s\n",cmds[1].desc);
}

// help 命令处理函数
int help_handler(int argc, char *argv[])
{
    int i;
    for(i=0;strlen(cmds[i].cmd)!=0;i++){
        if(streq(argv[1],cmds[i].cmd)){
            if(cmds[i].help_func) cmds[i].help_func();
            else myPrintk(0x7,"Help: No Help Information\n");
            return i;
        }
    }
    myPrintk(0x7,"Help: Unknown Command\n");
	return 0;
}

// cmd 命令处理函数
int cmd_handler(int argc, char **argv){
    int i;
    for(i=0;strlen(cmds[i].cmd)!=0;i++){
        myPrintk(0x7,"%s: %s\n",cmds[i].cmd,cmds[i].desc);
    }
    return i;
}

// cls 命令处理函数
int cls_handler(int argc, char **argv){
    clear_screen();
    return 1;
}


// 主命令处理函数
int handler(int argc, char **argv)
{
    if(strlen(argv[0])==0) return 0;
    int i;
    for(i=0;strlen(cmds[i].cmd)!=0;i++){
        if(streq(argv[0],cmds[i].cmd)){
            cmds[i].func(argc,argv);
            return i;
        }
    }
    myPrintk(0x7,"Unknown Command\n");
    return 0;

	
}


// 通过uart读入命令
static char buf[MAXSIZE];
char* read(void){
    for(int i=0;i<MAXSIZE;i++) buf[i]=0;//init buffer
    char c;
    int cursor = 0;
    /* read from uart */
    while(cursor<MAXSIZE-1 && (c=uart_get_char())!=13){
        uartPrintf("%c",c);
        switch (c){
        /* backspace */
        case 127:
            if(cursor>0) buf[--cursor]=0;
            break;
        default:
            buf[cursor++]=c;
            break;
        }
        show_input(0x7,buf);
    }
    myPrintk(0x7,"%s\n",buf);
    return buf;
}
void startShell(void)
{
    while(1){
        myPrintk(0xa,"shell@myos:");
        char* cmd;
        cmd=read();
        parseCmdStr(cmd);
        handler(parameters.argc,parameters.argv);
    }
}
