.text
.code32 

  .globl CTX_SW
  .globl CTX_SW_IRET
CTX_SW:
	pushf 
  	pusha
	# switch current task
	movl currTSK, %eax
	movl nextTSK, %ebx
	movl %ebx, (%eax)
	# switch stack pointer
	movl prevTSK_StackPtr, %eax
	movl %esp, (%eax)
	movl nextTSK_StackPtr, %esp
	popa 
	popf
	ret
