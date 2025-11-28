#include <stdint.h>
#include "drivers/console.h"
#include "drivers/keyboard.h"
#include "memory.h"
#include "fat16.h"
#include "shell.h"

#define WINDOS_VERSION "4.0"
#define WINDOS_COPYRIGHT "Copyright (c) 2024 WinDos Project"

typedef struct {
    uint32_t total_memory;
    uint32_t free_memory;
    uint8_t drive_count;
    uint8_t fat_initialized;
} system_info_t;

system_info_t sys_info;

void system_init() {
    console_init();
    keyboard_init();
    memory_init();
    
    if (fat16_init(0) == 0) {
        sys_info.fat_initialized = 1;
        printf("FAT16 File System: OK\n");
    }
    
    sys_info.total_memory = detect_memory();
    sys_info.free_memory = sys_info.total_memory;
    
    printf("WinDos 4.0 Kernel Initialized\n");
    printf("Memory: %d KB available\n", sys_info.total_memory / 1024);
}

void kernel_main() {
    system_init();
    
    printf("\nWinDos 4.0 Ready\n");
    printf("Type 'HELP' for commands\n\n");
    
    shell_start();
    
    while(1) {
        asm volatile ("hlt");
    }
}
