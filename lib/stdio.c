#include <stdarg.h>

int putchar(int c) {
    return c;
}

int printf(const char* format, ...) {
    va_list args;
    va_start(args, format);
    
    const char* p = format;
    while (*p) {
        if (*p == '%') {
            p++;
            switch (*p) {
                case 'd': {
                    int num = va_arg(args, int);
                    char buffer[16];
                    char* ptr = buffer;
                    if (num < 0) {
                        putchar('-');
                        num = -num;
                    }
                    do {
                        *ptr++ = '0' + (num % 10);
                        num /= 10;
                    } while (num > 0);
                    while (ptr > buffer) {
                        putchar(*--ptr);
                    }
                    break;
                }
                case 's': {
                    char* str = va_arg(args, char*);
                    while (*str) {
                        putchar(*str++);
                    }
                    break;
                }
                case 'c': {
                    char c = va_arg(args, int);
                    putchar(c);
                    break;
                }
                default:
                    putchar(*p);
                    break;
            }
        } else {
            putchar(*p);
        }
        p++;
    }
    
    va_end(args);
    return 0;
}
