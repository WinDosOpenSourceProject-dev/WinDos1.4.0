#include "string.h"
#include "fat16.h"
#include "drivers/rtc.h"
#include "installer.h"
#include "save_state.h"

void cmd_copy(const char* source, const char* destination) {
    if (!source || !destination) {
        printf("Syntax: COPY source destination\n");
        return;
    }
    
    char buffer[1024];
    int size = fat16_read_file(source, buffer, sizeof(buffer));
    
    if (size < 0) {
        printf("File not found: %s\n", source);
        return;
    }
    
    if (fat16_create_file(destination, buffer, size) == 0) {
        printf("1 file(s) copied\n");
    } else {
        printf("Error copying file\n");
    }
}

void cmd_del(const char* filename) {
    if (!filename) {
        printf("Syntax: DEL filename\n");
        return;
    }
    
    if (fat16_delete_file(filename) == 0) {
        printf("File deleted: %s\n", filename);
    } else {
        printf("File not found: %s\n", filename);
    }
}

void cmd_move(const char* source, const char* destination) {
    if (!source || !destination) {
        printf("Syntax: MOVE source destination\n");
        return;
    }
    
    char buffer[1024];
    int size = fat16_read_file(source, buffer, sizeof(buffer));
    
    if (size < 0) {
        printf("File not found: %s\n", source);
        return;
    }
    
    if (fat16_create_file(destination, buffer, size) == 0) {
        fat16_delete_file(source);
        printf("1 file(s) moved\n");
    } else {
        printf("Error moving file\n");
    }
}

void cmd_cd(const char* dirname) {
    if (!dirname) {
        printf("Current directory: %s\n", current_directory);
        return;
    }
    
    if (strcmp(dirname, "..") == 0) {
        char* last_slash = strrchr(current_directory, '\\');
        if (last_slash && last_slash != current_directory + 2) {
            *last_slash = '\0';
        }
    } else if (strcmp(dirname, "\\") == 0) {
        strcpy(current_directory, "C:\\");
    } else {
        if (fat16_exists(dirname)) {
            if (strlen(current_directory) > 3) {
                strcat(current_directory, "\\");
            }
            strcat(current_directory, dirname);
        } else {
            printf("Directory not found: %s\n", dirname);
        }
    }
}

void cmd_md(const char* dirname) {
    if (!dirname) {
        printf("Syntax: MD directory_name\n");
        return;
    }
    
    if (fat16_create_directory(dirname) == 0) {
        printf("Directory created: %s\n", dirname);
    } else {
        printf("Error creating directory\n");
    }
}

void cmd_rd(const char* dirname) {
    if (!dirname) {
        printf("Syntax: RD directory_name\n");
        return;
    }
    
    if (fat16_delete_directory(dirname) == 0) {
        printf("Directory removed: %s\n", dirname);
    } else {
        printf("Error removing directory\n");
    }
}

void cmd_deltree(const char* dirname) {
    if (!dirname) {
        printf("Syntax: DELTREE directory_name\n");
        return;
    }
    
    printf("All files and subdirectories in %s will be deleted!\n", dirname);
    printf("Are you sure (Y/N)? ");
    
    char response = keyboard_getchar();
    putchar(response);
    putchar('\n');
    
    if (response == 'Y' || response == 'y') {
        if (fat16_delete_directory(dirname) == 0) {
            printf("Directory deleted: %s\n", dirname);
        } else {
            printf("Error deleting directory\n");
        }
    } else {
        printf("Delete cancelled\n");
    }
}

void cmd_print(const char* filename) {
    if (!filename) {
        printf("Syntax: PRINT filename\n");
        return;
    }
    
    char buffer[1024];
    int size = fat16_read_file(filename, buffer, sizeof(buffer));
    
    if (size < 0) {
        printf("File not found: %s\n", filename);
        return;
    }
    
    printf("Printing: %s\n", filename);
    printf("-------- Content --------\n");
    printf("%s\n", buffer);
    printf("-------- End --------\n");
    printf("File sent to printer\n");
}

void cmd_date(const char* new_date) {
    rtc_time_t time;
    rtc_get_time(&time);
    
    if (!new_date) {
        printf("Current date: %02d-%02d-%04d\n", time.day, time.month, time.year);
        printf("Enter new date (dd-mm-yyyy): ");
        
        char input[11];
        int pos = 0;
        while (pos < 10) {
            char c = keyboard_getchar();
            if (c == '\r') break;
            if ((c >= '0' && c <= '9') || c == '-') {
                putchar(c);
                input[pos++] = c;
            }
        }
        input[pos] = '\0';
        putchar('\n');
        
        if (pos > 0) {
            int day, month, year;
            if (sscanf(input, "%d-%d-%d", &day, &month, &year) == 3) {
                time.day = day;
                time.month = month;
                time.year = year;
                rtc_set_time(&time);
                printf("Date updated\n");
            } else {
                printf("Invalid date format\n");
            }
        }
    } else {
        int day, month, year;
        if (sscanf(new_date, "%d-%d-%d", &day, &month, &year) == 3) {
            time.day = day;
            time.month = month;
            time.year = year;
            rtc_set_time(&time);
            printf("Date updated to: %02d-%02d-%04d\n", day, month, year);
        } else {
            printf("Invalid date format. Use: DATE dd-mm-yyyy\n");
        }
    }
}

void cmd_time(const char* new_time) {
    rtc_time_t time;
    rtc_get_time(&time);
    
    if (!new_time) {
        printf("Current time: %02d:%02d:%02d\n", time.hour, time.minute, time.second);
        printf("Enter new time (hh:mm:ss): ");
        
        char input[9];
        int pos = 0;
        while (pos < 8) {
            char c = keyboard_getchar();
            if (c == '\r') break;
            if ((c >= '0' && c <= '9') || c == ':') {
                putchar(c);
                input[pos++] = c;
            }
        }
        input[pos] = '\0';
        putchar('\n');
        
        if (pos > 0) {
            int hour, minute, second;
            if (sscanf(input, "%d:%d:%d", &hour, &minute, &second) == 3) {
                time.hour = hour;
                time.minute = minute;
                time.second = second;
                rtc_set_time(&time);
                printf("Time updated\n");
            } else {
                printf("Invalid time format\n");
            }
        }
    } else {
        int hour, minute, second;
        if (sscanf(new_time, "%d:%d:%d", &hour, &minute, &second) == 3) {
            time.hour = hour;
            time.minute = minute;
            time.second = second;
            rtc_set_time(&time);
            printf("Time updated to: %02d:%02d:%02d\n", hour, minute, second);
        } else {
            printf("Invalid time format. Use: TIME hh:mm:ss\n");
        }
    }
}

void cmd_con(const char* filename) {
    if (!filename) {
        printf("Syntax: CON filename\n");
        printf("Type file content. Press F6 then Enter to save.\n");
        return;
    }
    
    printf("Creating file: %s\n", filename);
    printf("Enter file contents (F6 then Enter to save and exit):\n");
    
    char buffer[4096] = "";
    int pos = 0;
    int saving = 0;
    
    while (!saving) {
        char c = keyboard_getchar();
        
        if (c == 0) {
            c = keyboard_getchar();
            
            if (c == 0x40) {
                printf("^Z");
                saving = 1;
            }
        }
        else if (c == '\r') {
            putchar('\n');
            if (pos < sizeof(buffer) - 2) {
                buffer[pos++] = '\n';
            }
        }
        else if (c == '\b') {
            if (pos > 0) {
                pos--;
                putchar('\b');
                putchar(' ');
                putchar('\b');
            }
        }
        else if (c >= 32 && c <= 126) {
            putchar(c);
            if (pos < sizeof(buffer) - 1) {
                buffer[pos++] = c;
            }
        }
    }
    
    putchar('\n');
    
    if (fat16_create_file(filename, buffer, pos) == 0) {
        printf("File saved: %s (%d bytes)\n", filename, pos);
    } else {
        printf("Error saving file!\n");
    }
}

void cmd_install() {
    printf("WinDos 4.0 Installation Utility\n");
    printf("WARNING: This will modify your disk structure!\n");
    printf("Make sure you have backups of important data!\n\n");
    
    install_system();
}

void cmd_save(const char* description) {
    if (!description) {
        printf("Syntax: SAVE \"description\"\n");
        printf("Saves current system state\n");
        return;
    }
    
    for (int slot = 0; slot < MAX_SAVE_SLOTS; slot++) {
        if (save_system_state(description, slot) == 0) {
            break;
        }
    }
}

void cmd_load(int slot) {
    if (load_system_state(slot) != 0) {
        printf("Use LOAD slot_number to load a saved state\n");
        printf("Use SAVELIST to see available saves\n");
    }
}

void cmd_savelist() {
    list_saved_states();
}

void cmd_savedel(int slot) {
    if (slot < 0 || slot >= MAX_SAVE_SLOTS) {
        printf("Syntax: SAVEDEL slot_number\n");
        printf("Slot must be between 0 and %d\n", MAX_SAVE_SLOTS - 1);
        return;
    }
    
    delete_saved_state(slot);
}
