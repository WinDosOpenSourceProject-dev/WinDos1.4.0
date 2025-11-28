#ifndef FAT16_H
#define FAT16_H

int fat16_init(uint8_t drive);
void fat16_list_directory();
int fat16_read_file(const char* filename, char* buffer, int max_size);
int fat16_create_file(const char* filename, const char* content, int size);
int fat16_delete_file(const char* filename);
int fat16_create_directory(const char* dirname);
int fat16_delete_directory(const char* dirname);
int fat16_exists(const char* name);
void cmd_dir();
void cmd_type(const char* filename);

#endif
