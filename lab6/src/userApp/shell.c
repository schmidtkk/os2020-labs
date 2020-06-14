#define NULL 0
#define MAXSIZE 256
#define MAXARGS 16
#define MAXARGLEN 32
#define MAXCMDS 128

#include "memTestCase.h"
#include "../myOS/userInterface.h"


//命令添加函数
void addNewCmd( unsigned char *cmd,								//命令名
				int (*func)(int argc, unsigned char **argv),	//命令入口
				void (*help_func)(void),						//该命令的help入口，可为空
				unsigned char* description);					//该命令的描述

// 命令处理函数
int cmd_handler(int, unsigned char **);
int help_handler(int, unsigned char **);
int cls_handler(int, unsigned char **);
int exit_handler(int, unsigned char **);

// 帮助处理函数
void help_help(void);

struct command {
	char *cmd;
	int (*func)(int argc, unsigned char **argv);
	void (*help_func)(void);
	char *desc;
}**cmds;

int cmdcnt;
int exit_flag;

// get length of a string
int strlen(unsigned char* str){
	int i;
	for(i=0;str[i]!=0;i++);
	return i;
}

// return whether str1 equals str2 (1=equal,2=not)
int streq(unsigned char* str1, unsigned char* str2){
	int len1,len2;
	len1=strlen(str1);
	len2=strlen(str2);
	if(len1!=len2) return 0;
	for(int i=0;i<len1;i++){
		if(str1[i]!=str2[i]) return 0;
	}
	return 1;
}

int strcpy(unsigned char* src, unsigned char* dest){
	int len=strlen(src);
	int i;
	for(int i=0;i<=len;i++){
		*(dest+i)=*(src+i);
	}
		
	return i;
}

struct param{
	int argc;
	unsigned char args[MAXARGS][MAXARGLEN];
	unsigned char *argv[MAXARGS];
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
	myPrintf(0xf,"%s\n",cmds[1]->desc);
}

// help 命令处理函数
int help_handler(int argc,unsigned char **argv)
{
	for(int i=0;i<cmdcnt;i++){
		if(streq(argv[1],cmds[i]->cmd)){
			if(cmds[i]->help_func) cmds[i]->help_func();
			else myPrintf(0xf,"Help: No Help Information\n");
			return i;
		}
	}
	myPrintf(0xf,"Help: You need to specify a COMMAND so as to get HELP INFORMATION.\n");
	return 0;
}

// cmd 命令处理函数
int cmd_handler(int argc, unsigned char **argv){
	int i;
	for(i=0;i<cmdcnt;i++){
		myPrintf(0xf,"%s: %s\n",cmds[i]->cmd,cmds[i]->desc);
	}
	return i;
}

// cls 命令处理函数
int cls_handler(int argc, unsigned char **argv){
	clear_screen();
	return 1;
}

// exit 命令处理函数
int exit_handler(int argc, unsigned char **argv){
	exit_flag = 1;
	myPrintf(0x5, "Shell ShutDown\n");
	return 1;
}

// 主命令处理函数
int handler(int argc, unsigned char **argv)
{
	if(strlen(argv[0])==0) return 0;
	for(int i=0;i<cmdcnt;i++){
		if(streq(argv[0],cmds[i]->cmd)){
		cmds[i]->func(argc,argv);
			return i;
		}
	}
	myPrintf(0xf,"Unknown Command\n");
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
		show_input(0xf,buf);
	}
	myPrintf(0xf,"%s\n",buf);
	return buf;
}
void addNewCmd( unsigned char *cmd,								//命令名
				int (*func)(int argc, unsigned char **argv),	//命令入口
				void (*help_func)(void),						//该命令的help入口，可为空
				unsigned char* description)						//该命令的描述
	
{

	cmds[cmdcnt] = (struct command *) malloc(sizeof(struct command));
	cmds[cmdcnt]->cmd = (unsigned char *)malloc(strlen(cmd));
	strcpy(cmd, cmds[cmdcnt]->cmd);
	cmds[cmdcnt]->func = func;
	cmds[cmdcnt]->help_func = help_func;
	cmds[cmdcnt]->desc = (unsigned char *)malloc(strlen(description));
	strcpy(description, cmds[cmdcnt]->desc);
	cmdcnt++;
}


void initShell(){
	cmds = (struct command **) malloc(sizeof(unsigned long) * MAXCMDS);
	addNewCmd("cmd", cmd_handler, NULL, "list all commands");
	addNewCmd("help", help_handler, help_help, "help [cmd]");
	addNewCmd("cls", cls_handler, NULL, "clear screen");
	addNewCmd("exit", exit_handler, NULL, "shutdown shell");

}
void startShell(void)
{
	memTestCaseInit();
	while(exit_flag==0){
		myPrintf(0xa,"Shell@myOS:");
		char* cmd;
		cmd=read();
		parseCmdStr(cmd);
		handler(parameters.argc,parameters.argv);
	}
}
