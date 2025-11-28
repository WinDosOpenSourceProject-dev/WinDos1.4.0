#ifndef CONSOLE_H
#define CONSOLE_H

void console_clear();
void console_init();
void putchar(char c);
void printf(const char* str);
unsigned char inb(unsigned short port);
void outb(unsigned short port, unsigned char data);

#endif
