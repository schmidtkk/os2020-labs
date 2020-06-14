#include <stdarg.h>
#include "../include/vga.h"
#include "../include/uart.h"
#include "../include/vsprintf.h"

char kbuf[400];
int myPrintk(int color, const char* format, ...) {
	va_list args;
	int i;
	va_start(args, format);
	//write formatted string to buffer
	i = vsprintf(kbuf, format, args);
	//release args
	va_end(args);
	//append formatted string to screen
	append2screen(kbuf, color);
	uart_put_chars(kbuf);
	return i;
}

char ubuf[400];
int myPrintf(int color, const char* format, ...) {
	va_list args;
	int i;
	va_start(args, format);
	//write formatted string to buffer
	i = vsprintf(ubuf, format, args);
	//release args
	va_end(args);
	//append formatted string to screen
	append2screen(ubuf, color);
	uart_put_chars(ubuf);
	return i;
}

char buf[400];
int uartPrintf(const char* format, ...) {
	va_list args;
	int i;
	va_start(args, format);
	//write formatted string to buffer
	i = vsprintf(buf, format, args);
	//release args
	va_end(args);
	//append formatted string to serial
	uart_put_chars(buf);
	return i;
}

char pbuf[400];
int putBottomRight(int color, const char* format, ...){
	va_list args;
	int i;
	va_start(args, format);
	//write formatted string to buffer
	i = vsprintf(pbuf, format, args);
	//release args
	va_end(args);
	//append formatted string to serial
	put_chars(pbuf,color,24,80-i);
	return i;	
}