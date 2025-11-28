#include <stdint.h>
#include "string.h"
#include "drivers/disk.h"
#include "fat16.h"

#define SAVE_SIGNATURE 0x57445353
#define MAX_SAVE_SLOTS 10
#define SAVE_SECTOR 1024

typedef struct {
    uint32_t signature;
    uint32_t slot_id;
    char description[32];
    uint32_t timestamp;
    uint32_t data_size;
    uint32_t checksum;
    uint8_t active;
    char reserved[64];
} save_header_t;

typedef struct {
    char current_directory[64];
    char command_history[10][256];
    int history_count;
    uint32_t memory_usage;
    uint8_t screen_mode;
} system_state_t;

int save_system_state(const char* description, int slot) {
    save_header_t header;
    system_state_t state;
    uint8_t sector_buffer[512];
    
    header.signature = SAVE_SIGNATURE;
    header.slot_id = slot;
    strncpy(header.description, description, 31);
    header.description[31] = '\0';
    header.timestamp = 0;
    header.data_size = sizeof(system_state_t);
    header.active = 1;
    
    strcpy(state.current_directory, current_directory);
    state.history_count = 0;
    state.memory_usage = 0;
    state.screen_mode = 3;
    
    header.checksum = calculate_checksum(&state, sizeof(system_state_t));
    
    memset(sector_buffer, 0, 512);
    memcpy(sector_buffer, &header, sizeof(save_header_t));
    memcpy(sector_buffer + 128, &state, sizeof(system_state_t));
    
    uint32_t save_sector = SAVE_SECTOR + (slot * 2);
    
    if (write_disk_sectors(0x80, save_sector, 1, sector_buffer) != 0) {
        printf("Error saving state to slot %d\n", slot);
        return -1;
    }
    
    printf("System state saved to slot %d: %s\n", slot, description);
    return 0;
}

int load_system_state(int slot) {
    save_header_t header;
    system_state_t state;
    uint8_t sector_buffer[512];
    
    uint32_t save_sector = SAVE_SECTOR + (slot * 2);
    
    if (read_disk_sectors(0x80, save_sector, 1, sector_buffer) != 0) {
        printf("Error reading save slot %d\n", slot);
        return -1;
    }
    
    memcpy(&header, sector_buffer, sizeof(save_header_t));
    
    if (header.signature != SAVE_SIGNATURE || !header.active) {
        printf("No valid save in slot %d\n", slot);
        return -1;
    }
    
    memcpy(&state, sector_buffer + 128, sizeof(system_state_t));
    uint32_t checksum = calculate_checksum(&state, sizeof(system_state_t));
    
    if (checksum != header.checksum) {
        printf("Save data corrupted in slot %d\n", slot);
        return -1;
    }
    
    strcpy(current_directory, state.current_directory);
    
    printf("System state loaded from slot %d: %s\n", slot, header.description);
    return 0;
}

void list_saved_states() {
    save_header_t header;
    uint8_t sector_buffer[512];
    
    printf("Saved system states:\n");
    printf("Slot | Description                | Status\n");
    printf("-----+----------------------------+--------\n");
    
    int found = 0;
    for (int slot = 0; slot < MAX_SAVE_SLOTS; slot++) {
        uint32_t save_sector = SAVE_SECTOR + (slot * 2);
        
        if (read_disk_sectors(0x80, save_sector, 1, sector_buffer) == 0) {
            memcpy(&header, sector_buffer, sizeof(save_header_t));
            
            if (header.signature == SAVE_SIGNATURE && header.active) {
                printf(" %2d  | %-26s | ACTIVE\n", slot, header.description);
                found++;
            }
        }
    }
    
    if (!found) {
        printf("No saved states found\n");
    }
}

int delete_saved_state(int slot) {
    uint8_t sector_buffer[512];
    
    uint32_t save_sector = SAVE_SECTOR + (slot * 2);
    
    if (read_disk_sectors(0x80, save_sector, 1, sector_buffer) != 0) {
        printf("Error accessing slot %d\n", slot);
        return -1;
    }
    
    save_header_t* header = (save_header_t*)sector_buffer;
    
    if (header->signature != SAVE_SIGNATURE || !header->active) {
        printf("No valid save in slot %d\n", slot);
        return -1;
    }
    
    header->active = 0;
    
    if (write_disk_sectors(0x80, save_sector, 1, sector_buffer) != 0) {
        printf("Error deleting slot %d\n", slot);
        return -1;
    }
    
    printf("Save slot %d deleted\n", slot);
    return 0;
}

uint32_t calculate_checksum(void* data, uint32_t size) {
    uint32_t checksum = 0;
    uint8_t* bytes = (uint8_t*)data;
    
    for (uint32_t i = 0; i < size; i++) {
        checksum += bytes[i];
    }
    
    return checksum;
}
