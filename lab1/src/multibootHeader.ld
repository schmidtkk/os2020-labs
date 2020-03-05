/* 输出32位对应i386架构处理器的可执行和可链接文件 */
OUTPUT_FORMAT("elf32-i386", "elf32-i386", "elf32-i386")
/* 指定目标架构为i386 */
OUTPUT_ARCH(i386)
/* 设置程序的入口为start位置 */
ENTRY(start)
/* 定义文件的结构 */
SECTIONS {
    . = 1M;                     /* .text的起始位置为1M */
    .text : {
        *(.multiboot_header)    /* 所有文件的.multiboot_header片段 */
        . = ALIGN(8);           /* 取下一个按8字节对齐的位置 */
        *(.text)                /* 所有文件的.text片段 */
    }
}