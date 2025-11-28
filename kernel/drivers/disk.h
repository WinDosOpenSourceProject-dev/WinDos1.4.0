#ifndef DISK_H
#define DISK_H

int read_disk_sectors(uint8_t drive, uint32_t lba, uint8_t count, uint8_t* buffer);
int write_disk_sectors(uint8_t drive, uint32_t lba, uint8_t count, uint8_t* buffer);
int get_disk_info(uint8_t drive, uint16_t* cylinders, uint16_t* heads, uint16_t* sectors);

#endif
