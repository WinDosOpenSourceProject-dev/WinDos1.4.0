#include <stdint.h>
#include "string.h"
#include "drivers/disk.h"
#include "fat16.h"

#define INSTALL_SIGNATURE 0x57444F53

typedef struct {
    uint32_t signature;
    uint32_t version;
    uint32_t kernel_sectors;
    uint32_t system_sectors;
    uint8_t installed;
    uint8_t boot_drive;
    char reserved[106];
} install_info_t;

install_info_t install_info;

const char* install_messages[] = {
    "WinDos 4.0 Installation",
    "=======================",
    "",
    "This will install WinDos 4.0 to your hard disk.",
    "All data on the target disk will be lost!",
    "",
    "Press I to install, C to cancel",
    NULL
};

int read_mbr(uint8_t drive, uint8_t* buffer) {
    return read_disk_sectors(drive, 0, 1, buffer);
}

int write_mbr(uint8_t drive, uint8_t* buffer) {
    return write_disk_sectors(drive, 0, 1, buffer);
}

void create_fat16_bootsector(uint8_t* buffer, uint32_t total_sectors) {
    fat16_boot_sector_t* bs = (fat16_boot_sector_t*)buffer;
    
    memset(bs, 0, sizeof(fat16_boot_sector_t));
    
    bs->jump[0] = 0xEB;
    bs->jump[1] = 0x3C;
    bs->jump[2] = 0x90;
    
    memcpy(bs->oem, "WinDos4.0", 9);
    bs->bytes_per_sector = 512;
    bs->sectors_per_cluster = 4;
    bs->reserved_sectors = 8;
    bs->fat_copies = 2;
    bs->root_entries = 512;
    bs->total_sectors = total_sectors < 65536 ? total_sectors : 0;
    bs->media_descriptor = 0xF8;
    bs->sectors_per_fat = 128;
    bs->sectors_per_track = 63;
    bs->number_of_heads = 16;
    bs->hidden_sectors = 8;
    bs->total_sectors_large = total_sectors >= 65536 ? total_sectors : 0;
    bs->drive_number = 0x80;
    bs->boot_signature = 0x29;
    bs->volume_id = 0x12345678;
    memcpy(bs->volume_label, "WinDos4.0  ", 11);
    memcpy(bs->file_system, "FAT16   ", 8);
    
    buffer[510] = 0x55;
    buffer[511] = 0xAA;
}

void create_fat_table(uint8_t* buffer) {
    memset(buffer, 0, 512);
    
    buffer[0] = 0xF8;
    buffer[1] = 0xFF;
    buffer[2] = 0xFF;
    buffer[3] = 0xFF;
}

int format_disk(uint8_t drive, uint32_t total_sectors) {
    uint8_t sector_buffer[512];
    
    printf("Formatting disk...\n");
    
    create_fat16_bootsector(sector_buffer, total_sectors);
    if (write_disk_sectors(drive, 0, 1, sector_buffer) != 0) {
        printf("Error writing boot sector\n");
        return -1;
    }
    
    for (int fat_copy = 0; fat_copy < 2; fat_copy++) {
        for (int sector = 0; sector < 128; sector++) {
            if (sector == 0) {
                create_fat_table(sector_buffer);
            } else {
                memset(sector_buffer, 0, 512);
            }
            
            uint32_t lba = 8 + (fat_copy * 128) + sector;
            if (write_disk_sectors(drive, lba, 1, sector_buffer) != 0) {
                printf("Error writing FAT\n");
                return -1;
            }
        }
    }
    
    memset(sector_buffer, 0, 512);
    uint32_t root_dir_start = 8 + (2 * 128);
    for (int sector = 0; sector < 32; sector++) {
        if (write_disk_sectors(drive, root_dir_start + sector, 1, sector_buffer) != 0) {
            printf("Error writing root directory\n");
            return -1;
        }
    }
    
    printf("Disk formatted successfully\n");
    return 0;
}

int copy_system_files(uint8_t drive) {
    printf("Copying system files...\n");
    
    const char* autoexec_content = 
        "@echo off\n"
        "echo WinDos 4.0\n"
        "ver\n"
        "dir\n";
    
    fat16_create_file("AUTOEXEC.BAT", autoexec_content, strlen(autoexec_content));
    fat16_create_file("COMMAND.COM", "WinDos Command Processor", 25);
    fat16_create_file("CONFIG.SYS", "FILES=20\nBUFFERS=15", 20);
    
    printf("System files copied\n");
    return 0;
}

int install_bootloader(uint8_t drive) {
    uint8_t mbr[512];
    uint8_t bootloader[512];
    
    printf("Installing bootloader...\n");
    
    if (read_mbr(drive, mbr) != 0) {
        printf("Error reading MBR\n");
        return -1;
    }
    
    memset(bootloader, 0, 512);
    
    bootloader[0] = 0xEB;
    bootloader[1] = 0x3C;
    bootloader[2] = 0x90;
    
    char* msg = "WinDos 4.0 Boot Manager";
    memcpy(bootloader + 0x3E, msg, strlen(msg));
    
    bootloader[510] = 0x55;
    bootloader[511] = 0xAA;
    
    if (write_mbr(drive, bootloader) != 0) {
        printf("Error writing MBR\n");
        return -1;
    }
    
    printf("Bootloader installed\n");
    return 0;
}

void install_system() {
    printf("\n");
    for (int i = 0; install_messages[i] != NULL; i++) {
        printf("%s\n", install_messages[i]);
    }
    printf("\n");
    
    char choice;
    do {
        choice = keyboard_getchar();
    } while (choice != 'I' && choice != 'i' && choice != 'C' && choice != 'c');
    
    putchar(choice);
    putchar('\n');
    
    if (choice == 'C' || choice == 'c') {
        printf("Installation cancelled\n");
        return;
    }
    
    printf("\nStarting installation...\n");
    
    uint8_t install_drive = 0x80;
    
    if (install_bootloader(install_drive) != 0) {
        printf("Installation failed at bootloader step\n");
        return;
    }
    
    if (format_disk(install_drive, 20480) != 0) {
        printf("Installation failed at formatting step\n");
        return;
    }
    
    if (copy_system_files(install_drive) != 0) {
        printf("Installation failed at file copy step\n");
        return;
    }
    
    install_info.signature = INSTALL_SIGNATURE;
    install_info.version = 0x00040000;
    install_info.kernel_sectors = 100;
    install_info.system_sectors = 200;
    install_info.installed = 1;
    install_info.boot_drive = install_drive;
    
    printf("\nWinDos 4.0 installed successfully!\n");
    printf("Please restart your computer.\n");
}
