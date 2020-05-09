#include "../../include/myPrintk.h"
#include "../../include/mem.h"
// 一个EEB表示一个空闲可用的Block
struct EEB {
	unsigned long next_start;
};

void showEEB(struct EEB *eeb){
	myPrintk(0x7,"EEB(start=0x%x, next=0x%x)\n", eeb, eeb->next_start);
}


void alignby8(unsigned long *num){
	if(*num&1) *num++;
	if(*num&2) *num+=2;
	if(*num&4) *num+=4;
}
//eFPartition是表示整个内存的数据结构
struct eFPartition{
	unsigned long totalN;
	unsigned long perSize;  //unit: byte	
	unsigned long firstFree;
};

void showeFPartition(struct eFPartition *efp){
	myPrintk(0x5,"eFPartition(start=0x%x, totalN=0x%x, perSize=0x%x, firstFree=0x%x)\n", efp, efp->totalN, efp->perSize, efp->firstFree);
}

void eFPartitionWalkByAddr(unsigned long efpHandler){

	struct eFPartition * efp = (struct eFPartition *) efpHandler;
	showeFPartition(efp);
	struct EEB * p = (struct EEB *) efp->firstFree;
	while(p){
		showEEB(p);
		p = (struct EEB *) p->next_start;
	}
}


unsigned long eFPartitionTotalSize(unsigned long perSize, unsigned long n){
	alignby8(&perSize);
	return perSize * n + sizeof(struct eFPartition);
}

unsigned long eFPartitionInit(unsigned long start, unsigned long perSize, unsigned long n){
	//Init efpHeader
	struct eFPartition *efp = (struct eFPartition *) start;
	alignby8(&perSize);
	efp->perSize = perSize;
	efp->totalN = n;
	efp->firstFree = start + sizeof(struct eFPartition);

	//Init EEB
	struct EEB * eeb;
	unsigned long addr = efp->firstFree;
	for(int i=0;i<n;i++){
		eeb = (struct EEB *) addr;
		eeb -> next_start = addr + perSize;
		addr += perSize;
	}
	eeb -> next_start = 0;
	eFPartitionWalkByAddr((unsigned long) efp);

	return (unsigned long)efp;
}


unsigned long eFPartitionAlloc(unsigned long EFPHandler){
	struct eFPartition * efp = (struct eFPartition *) EFPHandler;
	struct EEB * eeb = (struct EEB *) efp->firstFree;
	
	//alloc fail return 0
	if(efp->firstFree > EFPHandler+eFPartitionTotalSize(efp->perSize,efp->totalN)) return 0;

	//alloc succeed return eeb handler
	efp->firstFree = eeb->next_start;
	return (unsigned long) eeb;
}


unsigned long eFPartitionFree(unsigned long EFPHandler,unsigned long mbStart){
	struct eFPartition * efp = (struct eFPartition *) EFPHandler;
	if(mbStart==0) mbStart = EFPHandler + eFPartitionTotalSize(efp->perSize,efp->totalN);
	efp->firstFree = EFPHandler + sizeof(struct eFPartition);
	struct EEB *eeb = (struct EEB *) efp->firstFree;
	int cnt = 0;
	//free all blocks ahead of mbStart
	while((unsigned long) eeb < mbStart){
		eeb -> next_start  = (unsigned long) eeb + efp->perSize;
		eeb = (struct EEB *) ((unsigned long) eeb + efp->perSize);
		cnt++;
	}
	eeb = (struct EEB *) ((unsigned long) eeb - efp->perSize);
	if(cnt==efp->totalN) eeb->next_start = 0;
	
	return 1;
}

// extern unsigned long pMemStart;
// void test(void){
// 	pMemInit();
// 	myPrintk(0xf,"0x%x\n",pMemStart);

// 	struct eFPartition * efp = (struct eFPartition *) eFPartitionInit(pMemStart,10,5);

// 	//alloc
// 	myPrintk(0xf,"Alloc Addr: 0x%x\n",eFPartitionAlloc((unsigned long) efp));
// 	myPrintk(0xf,"Alloc Addr: 0x%x\n",eFPartitionAlloc((unsigned long) efp));
// 	myPrintk(0xf,"Alloc Addr: 0x%x\n",eFPartitionAlloc((unsigned long) efp));

// 	eFPartitionWalkByAddr((unsigned long) efp);
// 	//free
// 	eFPartitionFree((unsigned long) efp, efp->firstFree);

// 	eFPartitionWalkByAddr((unsigned long) efp);



// }