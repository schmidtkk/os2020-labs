.global  start
.section .multiboot_header
.long 0x1BADB002        /* magic        */
.long 0x0               /* flags        */
.long -0x1BADB002       /* checksum     */

.section .text
start:
        movw $0x3f8, %dx
        nop
        nop

        movb $0x68, %al         /* putchar h    */
        outb %al, %dx
        movb $0x65, %al         /* putchar e    */
        outb %al, %dx
        movb $0x6c, %al         /* putchar l    */
        outb %al, %dx
        movb $0x6c, %al         /* putchar l    */
        outb %al, %dx
        movb $0x6f, %al         /* putchar o    */
        outb %al, %dx
        movb $0x77, %al         /* putchar w    */
        outb %al, %dx
        movb $0x6f, %al         /* putchar o    */
        outb %al, %dx
        movb $0x72, %al         /* putchar r    */
        outb %al, %dx
        movb $0x6c, %al         /* putchar l    */
        outb %al, %dx
        movb $0x64, %al         /* putchar d    */
        outb %al, %dx

helloworld:
	movl $0x2f652f68, 0xB8000
        movl $0x2f6c2f6c, 0xB8004
        movl $0x2f772f6f, 0xB8008
        movl $0x2f722f6f, 0xB800C
        movl $0x2f642f6c, 0xB8010
        movl $0x2f202f2c, 0xB8014
	movl $0x2f422f50, 0xB8018
	movl $0x2f382f31, 0xB801C
        movl $0x2f312f31, 0xB8020
        movl $0x2f362f31, 0xB8024
        movl $0x2f302f38, 0xB8028
	movl $0x2f472f5f, 0xB802C
	movl $0x2f6f2f75, 0xB8030
	movl $0x2f652f57, 0xB8034
	movl $0x2f642f69, 0xB8038
	movl $0x2f6e2f6f, 0xB803C
	movl $0x00002f67, 0xB8040

clean:
        movl $0, %ebx
loop:
        movl $0xB8044, %eax
        movl $0 , (%eax, %ebx, 4)
        addl $1, %ebx
        cmpl $0x1000, %ebx
        jl loop
        
	hlt

