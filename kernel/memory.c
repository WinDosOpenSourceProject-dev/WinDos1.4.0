#include <stdint.h>

#define MEMORY_MAP_SIZE 1024
#define PAGE_SIZE 4096

typedef struct {
    uint32_t base_low;
    uint32_t base_high;
    uint32_t length_low;
    uint32_t length_high;
    uint32_t type;
    uint32_t attributes;
} memory_map_entry_t;

memory_map_entry_t memory_map[MEMORY_MAP_SIZE];
int memory_map_entries = 0;

void memory_init() {
    asm volatile (
        "int $0x15"
        : "=c"(memory_map_entries)
        : "a"(0xE820), "b"(0), "c"(24), "d"(0x534D4150)
        : "memory"
    );
}

uint32_t detect_memory() {
    uint32_t total = 0;
    
    for (int i = 0; i < memory_map_entries; i++) {
        if (memory_map[i].type == 1) {
            total += memory_map[i].length_low;
        }
    }
    
    return total;
}

uint32_t memory_alloc(uint32_t size) {
    static uint32_t next_free = 0x100000;
    
    uint32_t allocated = next_free;
    next_free += size;
    
    if (next_free % PAGE_SIZE != 0) {
        next_free += PAGE_SIZE - (next_free % PAGE_SIZE);
    }
    
    return allocated;
}
