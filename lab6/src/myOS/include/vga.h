#ifndef __VGA_H__
#define __VGA_H__

void append_char(char c, int color);
void clear_screen(void);
void append2screen(char* str, int color);
void put_chars(char* str, int color, int row, int col);
void show_input(int color, char* str);

#endif