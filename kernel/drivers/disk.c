#include <stdint.h>

int read_disk_sectors(uint8_t drive, uint32_t lba, uint8_t count, uint8_t* buffer) {
    uint16_t retry = 3;
    
    while (retry--) {
        asm volatile (
            "pusha\n"
            "mov $0x42, %ah\n"
            "mov %0, %dl\n"
            "mov %1, %si\n"
            "int $0x13\n"
            "jnc success\n"
            "mov $1, %0\n"
            "jmp end\n"
            "success:\n"
            "mov $0, %0\n"
            "end:\n"
            "popa"
            : "=r"(retry)
            : "r"(drive), "r"(lba), "r"(count), "r"(buffer)
            : "memory"
        );
        
        if (retry == 0) break;
    }
    
    return retry;
}

int write_disk_sectors(uint8_t drive, uint32_t lba, uint8_t count, uint8_t* buffer) {
    uint16_t retry = 3;
    
    while (retry--) {
        asm volatile (
            "pusha\n"
            "mov $0x43, %ah\n"
            "mov %0, %dl\n"
            "mov %1, %si\n"
            "int $0x13\n"
            "jnc success\n"
            "mov $1, %0\n"
            "jmp end\n"
            "success:\n"
            "mov $0, %0\n"
            "end:\n"
            "popa"
            : "=r"(retry)
            : "r"(drive), "r"(lba), "r"(count), "r"(buffer)
            : "memory"
        );
        
        if (retry == 0) break;
    }
    
    return retry;
}

int get_disk_info(uint8_t drive, uint16_t* cylinders, uint16_t* heads, uint16_t* sectors) {
    asm volatile (
        "pusha\n"
        "mov $0x08, %ah\n"
        "mov %0, %dl\n"
        "int $0x13\n"
        "jc error\n"
        "mov %%cx, %1\n"
        "mov %%dx, %2\n"
        "mov $0, %0\n"
        "jmp end\n"
        "error:\n"
        "mov $1, %0\n"
        "end:\n"
        "popa"
        : "=r"(drive)
        : "r"(cylinders), "r"(heads)
        : "memory"
    );
    
    if (drive == 0) {
        *sectors = *cylinders & 0x3F;
        *cylinders = ((*cylinders >> 8) & 0xFF) | ((*cylinders & 0xC0) << 2);
        *heads = (*heads >> 8) & 0xFF;
    }
    
    return drive;
}
