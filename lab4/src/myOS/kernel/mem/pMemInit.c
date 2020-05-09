#include "../../include/myPrintk.h"
extern unsigned long dPartitionInit(unsigned long start, unsigned long size);

unsigned long pMemStart;//可用的内存的起始地址
unsigned long pMemSize;//可用的大小
unsigned long pMemHandler;//内存分区头指针


void writeWord(unsigned long addr, short write){
	__asm__ __volatile__("movw %0,(%1)"::"a"(write),"b"(addr));
}
void readWord(unsigned long addr, short *read){
	__asm__ __volatile__("movw (%1),%0":"=a"(*read):"b"(addr));
}

void memTest(unsigned long start, unsigned long grainSize){

	unsigned long addr,step,tail,total;
	addr = start<0x400?0x400:start;
	tail = addr + grainSize - 2;
	step = grainSize<2?2:grainSize;
	int flag;
	short data,check,write1,write2;
	total=-1;
	write1=0x55AA;
	write2=0xAA55;
	

	flag=0;
	while(flag!=4){
		flag=0;
		readWord(addr,&data);
		writeWord(addr,write1);
		readWord(addr,&check);
		flag += (check==write1);
		writeWord(addr,write2);
		readWord(addr,&check);
		flag += (check==write2);
		writeWord(addr,data);


		readWord(tail,&data);
		writeWord(tail,write1);
		readWord(tail,&check);
		flag += (check==write1);
		writeWord(tail,write2);
		readWord(tail,&check);
		flag += (check==write2);
		writeWord(tail,data);

		if(flag!=4) addr+=grainSize;
	}

	pMemStart = addr;

	flag=4;
	while(flag==4){
		flag=0;
		readWord(addr,&data);
		writeWord(addr,write1);
		readWord(addr,&check);
		flag += (check==write1);
		writeWord(addr,write2);
		readWord(addr,&check);
		flag += (check==write2);
		writeWord(addr,data);


		readWord(tail,&data);
		writeWord(tail,write1);
		readWord(tail,&check);
		flag += (check==write1);
		writeWord(tail,write2);
		readWord(tail,&check);
		flag += (check==write2);
		writeWord(tail,data);

		if(flag==4){
			addr+=grainSize;
			pMemSize+=grainSize;
		}
	}	
}

extern unsigned long _end;

void pMemInit(void){
	unsigned long _end_addr = (unsigned long) &_end;
	memTest(0x100000,0x1000);
	if (pMemStart <= _end_addr) {
		pMemSize -= _end_addr - pMemStart;
		pMemStart = _end_addr;
	}
	myPrintk(0xf,"MemStart: %x  \n",pMemStart);
	myPrintk(0xf,"MemSize:  %x  \n\n\n",pMemSize);

	pMemHandler = dPartitionInit(pMemStart,pMemSize);	
}
