#include <stdint.h>

#define KEYBOARD_PORT 0x60

const char scancode_table[] = {
    0, 0, '1', '2', '3', '4', '5', '6', '7', '8', '9', '0', '-', '=', 0,
    0, 'q', 'w', 'e', 'r', 't', 'y', 'u', 'i', 'o', 'p', '[', ']', '\n',
    0, 'a', 's', 'd', 'f', 'g', 'h', 'j', 'k', 'l', ';', '\'', '`', 0,
    '\\', 'z', 'x', 'c', 'v', 'b', 'n', 'm', ',', '.', '/', 0, '*', 0,
    ' ', 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
};

char keyboard_buffer[256];
int buffer_pos = 0;

void keyboard_handler() {
    uint8_t scancode = inb(KEYBOARD_PORT);
    
    if (scancode < sizeof(scancode_table) && scancode_table[scancode] != 0) {
        if (buffer_pos < sizeof(keyboard_buffer) - 1) {
            keyboard_buffer[buffer_pos++] = scancode_table[scancode];
        }
    }
}

void keyboard_init() {
    asm volatile ("cli");
    asm volatile ("sti");
}

char keyboard_getchar() {
    while (buffer_pos == 0) {
        asm volatile ("hlt");
    }
    
    char c = keyboard_buffer[0];
    for (int i = 1; i < buffer_pos; i++) {
        keyboard_buffer[i-1] = keyboard_buffer[i];
    }
    buffer_pos--;
    
    return c;
}
