extern unsigned long dPartitionAlloc(unsigned long dp, unsigned long size);
extern unsigned long dPartitionFree( unsigned long dp, unsigned long start);

unsigned long pMemHandler;

unsigned long malloc(unsigned long size){
	//dPartition's Alloc
	if(pMemHandler) return dPartitionAlloc(pMemHandler,size);
	else return 0;
}

unsigned long free(unsigned long start){
	//dPartition's Free
	if(pMemHandler) return dPartitionFree(pMemHandler, start);
	else return 0;
}

unsigned long kmalloc(unsigned long size){
	//dPartition's Alloc
	if(pMemHandler) return dPartitionAlloc(pMemHandler,size);
	else return 0;
}

unsigned long kfree(unsigned long start){
	//dPartition's Free
	if(pMemHandler) return dPartitionFree(pMemHandler, start);
	else return 0;
}