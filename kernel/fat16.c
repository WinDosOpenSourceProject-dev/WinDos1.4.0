#include <stdint.h>
#include "string.h"
#include "memory.h"

#pragma pack(push, 1)
typedef struct {
    uint8_t jump[3];
    char oem[8];
    uint16_t bytes_per_sector;
    uint8_t sectors_per_cluster;
    uint16_t reserved_sectors;
    uint8_t fat_copies;
    uint16_t root_entries;
    uint16_t total_sectors;
    uint8_t media_descriptor;
    uint16_t sectors_per_fat;
    uint16_t sectors_per_track;
    uint16_t number_of_heads;
    uint32_t hidden_sectors;
    uint32_t total_sectors_large;
    uint8_t drive_number;
    uint8_t current_head;
    uint8_t boot_signature;
    uint32_t volume_id;
    char volume_label[11];
    char file_system[8];
} fat16_boot_sector_t;

typedef struct {
    char filename[8];
    char extension[3];
    uint8_t attributes;
    uint8_t reserved[10];
    uint16_t time;
    uint16_t date;
    uint16_t first_cluster;
    uint32_t file_size;
} fat16_dir_entry_t;
#pragma pack(pop)

fat16_boot_sector_t boot_sector;
uint8_t fat_initialized = 0;
char current_directory[64] = "C:\\";

typedef struct {
    char name[13];
    char content[1024];
    uint32_t size;
    uint8_t is_directory;
} virtual_file_t;

virtual_file_t virtual_files[] = {
    {"AUTOEXEC.BAT", "echo WinDos 4.0\nver\ndir", 24, 0},
    {"CONFIG.SYS", "FILES=20\nBUFFERS=10", 21, 0},
    {"README.TXT", "Welcome to WinDos 4.0!", 22, 0},
    {"COMMAND.COM", "Internal Command Processor", 26, 0},
    {"DOS", "", 0, 1},
    {"GAMES", "", 0, 1},
    {"SYSTEM", "", 0, 1}
};
int virtual_file_count = 7;

int read_sector(uint32_t lba, uint8_t* buffer) {
    return 0;
}

int write_sector(uint32_t lba, uint8_t* buffer) {
    return 0;
}

int fat16_init(uint8_t drive) {
    fat_initialized = 1;
    return 0;
}

virtual_file_t* find_file(const char* filename) {
    for (int i = 0; i < virtual_file_count; i++) {
        if (strcmp(virtual_files[i].name, filename) == 0) {
            return &virtual_files[i];
        }
    }
    return NULL;
}

void cmd_dir() {
    if (!fat_initialized) {
        printf("File system not available\n");
        return;
    }
    fat16_list_directory();
}

void fat16_list_directory() {
    printf(" Directory of %s\n\n", current_directory);
    
    int dir_count = 0;
    int file_count = 0;
    uint32_t total_size = 0;
    
    for (int i = 0; i < virtual_file_count; i++) {
        if (virtual_files[i].is_directory) {
            printf("%-12s   <DIR>\n", virtual_files[i].name);
            dir_count++;
        } else {
            printf("%-12s   %8d bytes\n", virtual_files[i].name, virtual_files[i].size);
            file_count++;
            total_size += virtual_files[i].size;
        }
    }
    
    printf("\n%14d File(s) %12d bytes\n", file_count, total_size);
    printf("%14d Dir(s)\n", dir_count);
}

int fat16_read_file(const char* filename, char* buffer, int max_size) {
    virtual_file_t* file = find_file(filename);
    if (file && !file->is_directory) {
        int size = file->size;
        if (size > max_size) size = max_size;
        memcpy(buffer, file->content, size);
        return size;
    }
    return -1;
}

int fat16_create_file(const char* filename, const char* content, int size) {
    if (virtual_file_count >= 50) return -1;
    
    virtual_file_t* new_file = &virtual_files[virtual_file_count++];
    strcpy(new_file->name, filename);
    
    if (content && size > 0) {
        memcpy(new_file->content, content, size);
        new_file->size = size;
    } else {
        new_file->size = 0;
    }
    
    new_file->is_directory = 0;
    return 0;
}

int fat16_delete_file(const char* filename) {
    for (int i = 0; i < virtual_file_count; i++) {
        if (strcmp(virtual_files[i].name, filename) == 0 && !virtual_files[i].is_directory) {
            for (int j = i; j < virtual_file_count - 1; j++) {
                virtual_files[j] = virtual_files[j + 1];
            }
            virtual_file_count--;
            return 0;
        }
    }
    return -1;
}

int fat16_create_directory(const char* dirname) {
    if (virtual_file_count >= 50) return -1;
    
    virtual_file_t* new_dir = &virtual_files[virtual_file_count++];
    strcpy(new_dir->name, dirname);
    new_dir->size = 0;
    new_dir->is_directory = 1;
    return 0;
}

int fat16_delete_directory(const char* dirname) {
    for (int i = 0; i < virtual_file_count; i++) {
        if (strcmp(virtual_files[i].name, dirname) == 0 && virtual_files[i].is_directory) {
            for (int j = i; j < virtual_file_count - 1; j++) {
                virtual_files[j] = virtual_files[j + 1];
            }
            virtual_file_count--;
            return 0;
        }
    }
    return -1;
}

int fat16_exists(const char* name) {
    return find_file(name) != NULL;
}
