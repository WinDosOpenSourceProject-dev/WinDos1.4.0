#include <stdint.h>

#define VIDEO_MEMORY 0xB8000

uint16_t* video_memory = (uint16_t*)VIDEO_MEMORY;
int cursor_x = 0;
int cursor_y = 0;

void console_clear() {
    for (int i = 0; i < 80 * 25; i++) {
        video_memory[i] = 0x0720;
    }
    cursor_x = cursor_y = 0;
}

void console_init() {
    console_clear();
}

void putchar(char c) {
    if (c == '\n') {
        cursor_x = 0;
        cursor_y++;
    } else {
        int position = cursor_y * 80 + cursor_x;
        video_memory[position] = 0x0700 | c;
        cursor_x++;
        if (cursor_x >= 80) {
            cursor_x = 0;
            cursor_y++;
        }
    }
    
    if (cursor_y >= 25) {
        for (int i = 0; i < 24 * 80; i++) {
            video_memory[i] = video_memory[i + 80];
        }
        for (int i = 24 * 80; i < 25 * 80; i++) {
            video_memory[i] = 0x0720;
        }
        cursor_y = 24;
    }
}

void printf(const char* str) {
    while (*str) {
        putchar(*str++);
    }
}

unsigned char inb(unsigned short port) {
    unsigned char result;
    __asm__ volatile ("inb %1, %0" : "=a"(result) : "Nd"(port));
    return result;
}

void outb(unsigned short port, unsigned char data) {
    __asm__ volatile ("outb %0, %1" : : "a"(data), "Nd"(port));
}
