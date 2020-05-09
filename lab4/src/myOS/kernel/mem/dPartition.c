#include "../../include/myPrintk.h"

#define HEADERSIZE 8
extern void alignby8(unsigned long *num);

//dPartition 是整个动态分区内存的数据结构
struct dPartition{
	unsigned long size;
	unsigned long firstFreeStart; 
};



void showdPartition(struct dPartition *dp){
	myPrintk(0x5,"dPartition(start=0x%x, size=0x%x, firstFreeStart=0x%x)\n", dp, dp->size,dp->firstFreeStart);
}


// EMB每一个block的数据结构，userdata可以暂时不用管。
struct EMB{
	unsigned long size;
	union {
		unsigned long nextStart;    // if free: pointer to next block
        unsigned long userData;		// if allocated, blongs to user
	};	                           
};

void showEMB(struct EMB * emb){
	myPrintk(0x3,"EMB(start=0x%x, size=0x%x, nextStart=0x%x)\n", emb, emb->size, emb->nextStart);
}

unsigned long dPartitionInit(unsigned long start, unsigned long totalSize){
	//return start if succeed, return 0 if fail

	//totalSize should be bigger
	if(totalSize < HEADERSIZE + HEADERSIZE + 8) return 0;

	//init dPHeader
	struct dPartition *dp = (struct dPartition *) start;
	dp -> size = totalSize;
	dp -> firstFreeStart = start + HEADERSIZE;
	//Init EMB
	struct EMB *emb = (struct EMB *)(dp -> firstFreeStart);
	emb -> size = totalSize - HEADERSIZE - HEADERSIZE;
	emb -> nextStart = 0;

	//init succeed
	return start;
}

void dPartitionWalkByAddr(unsigned long dp){
	struct dPartition *dPart = (struct dPartition *) dp;
	struct EMB *emb = (struct EMB *)  dPart -> firstFreeStart;

	showdPartition(dPart);

	while(emb){
		showEMB(emb);
		emb = (struct EMB *) emb -> nextStart;
	}

}

//=================firstfit, order: address, low-->high=====================
/**
 * return value: addr (without overhead, can directly used by user)
**/
unsigned long dPartitionAllocFirstFit(unsigned long dp, unsigned long size){

	//illegal size
	if(size <= 0) return 0;


	struct dPartition *dPart = (struct dPartition *) dp;
	struct EMB *pre = 0;
	struct EMB *emb = (struct EMB *) dPart -> firstFreeStart;

	size += 4 ;//add at least 4-byte fence between embs
	alignby8(&size);

	while(emb){
		
		//allocate with current emb?
		if(emb->size >= size){
			//split current emb?
			if(emb -> size - size >= HEADERSIZE + 8){

				struct EMB *next = (struct EMB *) ((unsigned long) emb + size + HEADERSIZE);
				next -> size = emb -> size - size - HEADERSIZE;
				next -> nextStart = emb -> nextStart;
				emb -> size = size;//update current emb

				if(pre == 0) dPart -> firstFreeStart = (unsigned long) next;
				else pre -> nextStart = (unsigned long) next;

				return (unsigned long) emb;

			}
			else{
				if(pre == 0) dPart -> firstFreeStart = (unsigned long) emb->nextStart;
				else pre -> nextStart = (unsigned long) emb->nextStart;

				return (unsigned long) emb;
			}
		}
		pre = emb;
		emb = (struct EMB *) emb -> nextStart;
	}

	return 0;
}

/**
 * return value: succed 1, fail 0
**/
unsigned long dPartitionFreeFirstFit(unsigned long dp, unsigned long start){

	struct dPartition *dPart = (struct dPartition *) dp;	
	struct EMB *emb = (struct EMB *) dPart -> firstFreeStart;
	struct EMB *tar = (struct EMB *) start;

	//illegal start	
	if(start < dp + HEADERSIZE || start >= dp + dPart -> size){
		myPrintk(0xf,"here\n");
		return 0;
	}


	unsigned long beg = (unsigned long) tar;
	unsigned long end = (unsigned long) tar + HEADERSIZE + tar -> size;

	struct EMB *pre, *next;
	pre = next = 0;


	while(emb){


		
		if((unsigned long) emb < (unsigned long) tar) pre = emb;
		else if((unsigned long) emb > (unsigned long) tar){
			next = emb;
			break;
		}

		emb = (struct EMB *) emb -> nextStart;
	}

		//merge to next
		if(next){
			if(end == (unsigned long) next){
				tar -> nextStart = next -> nextStart;
				tar -> size += next -> size + HEADERSIZE;
			}
			else tar -> nextStart = (unsigned long) next;
		}
		else tar -> nextStart = 0;

		//merge to pre
		if(pre){
			if(beg == (unsigned long) pre + HEADERSIZE + pre -> size){
				pre -> nextStart = tar -> nextStart;
				pre -> size += tar -> size + HEADERSIZE;
			}
			else pre -> nextStart = (unsigned long) tar;
		}
		else dPart -> firstFreeStart = (unsigned long) tar;

		//free succeed
		return 1;
		


	
	
}

//wrap: we select firstFit, you can select another one
//user need not know this
unsigned long dPartitionAlloc(unsigned long dp, unsigned long size){
	return dPartitionAllocFirstFit(dp,size);
}

unsigned long dPartitionFree(unsigned long	 dp, unsigned long start){
	return dPartitionFreeFirstFit(dp,start);
}

extern unsigned long pMemStart;
extern void pMemInit(void);

void test(void){
	pMemInit();
	myPrintk(0xf,"0x%x\n",pMemStart);

	struct dPartition *dp = (struct dPartition *) dPartitionInit(pMemStart,128+16);

	dPartitionWalkByAddr((unsigned long) dp);

	unsigned long a,b,c,d;
	myPrintk(0xf, "\nFirstFit Alloc 1\n");
	a = dPartitionAllocFirstFit((unsigned long) dp, 16);
	dPartitionWalkByAddr((unsigned long) dp);

	myPrintk(0xf, "\nFirstFit Alloc 2\n");
	b= dPartitionAllocFirstFit((unsigned long) dp, 16);
	dPartitionWalkByAddr((unsigned long) dp);

	myPrintk(0xf, "\nFirstFit Alloc 3\n");
	c = dPartitionAllocFirstFit((unsigned long) dp, 16);
	dPartitionWalkByAddr((unsigned long) dp);

	myPrintk(0xf, "\nFirstFit Alloc 4\n");
	d = dPartitionAllocFirstFit((unsigned long) dp, 16);
	dPartitionWalkByAddr((unsigned long) dp);

	myPrintk(0x7, "\n\n0x%x, 0x%x, 0x%x, 0x%x\n\n",a,b,c,d);


	myPrintk(0xf, "\nFirstFit Free 1\n");
	a = dPartitionFree((unsigned long) dp, a);
	dPartitionWalkByAddr((unsigned long) dp);

	myPrintk(0xf, "\nFirstFit Free 2\n");
	b = dPartitionFree((unsigned long) dp, b);
	dPartitionWalkByAddr((unsigned long) dp);

	myPrintk(0xf, "\nFirstFit Free 3\n");
	c = dPartitionFree((unsigned long) dp, c);
	dPartitionWalkByAddr((unsigned long) dp);

	myPrintk(0xf, "\nFirstFit Free 4\n");
	d = dPartitionFree((unsigned long) dp, d);
	dPartitionWalkByAddr((unsigned long) dp);




	myPrintk(0x7, "\n\n%d, %d, %d\n\n",a,b,c);

}