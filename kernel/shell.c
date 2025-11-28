#include "string.h"
#include "drivers/keyboard.h"

#define MAX_COMMAND_LENGTH 256
#define MAX_ARGS 10

char command_buffer[MAX_COMMAND_LENGTH];
int command_pos = 0;
char current_directory[64] = "C:\\";

void cmd_help() {
    printf("WinDos 4.0 Commands:\n");
    printf("HELP     - This help\n");
    printf("VER      - Show version\n");
    printf("DIR      - List files\n");
    printf("TYPE     - Show file content\n");
    printf("COPY     - Copy files\n");
    printf("DEL      - Delete files\n");
    printf("MOVE     - Move files\n");
    printf("CD       - Change directory\n");
    printf("MD       - Create directory\n");
    printf("RD       - Remove directory\n");
    printf("DELTREE  - Remove directory tree\n");
    printf("PRINT    - Print file content\n");
    printf("DATE     - Show/set date\n");
    printf("TIME     - Show/set time\n");
    printf("CON      - Create/edit file\n");
    printf("INSTALL  - Install system\n");
    printf("SAVE     - Save system state\n");
    printf("LOAD     - Load system state\n");
    printf("SAVELIST - List saved states\n");
    printf("SAVEDEL  - Delete saved state\n");
    printf("MEM      - Memory info\n");
    printf("CLS      - Clear screen\n");
    printf("REBOOT   - Restart system\n");
}

void cmd_ver() {
    printf("Microsoft WinDos 4.0\n");
    printf("Copyright (c) 2024 WinDos Project\n");
}

void cmd_cls() {
    console_clear();
}

void cmd_mem() {
    printf("Memory: 640KB base memory detected\n");
}

void cmd_reboot() {
    printf("Rebooting system...\n");
    __asm__ volatile ("int $0x19");
}

void shell_prompt() {
    printf("%s>", current_directory);
}

void parse_command(char* cmd) {
    char* args[MAX_ARGS];
    int arg_count = 0;
    
    char* token = strtok(cmd, " ");
    while (token && arg_count < MAX_ARGS) {
        for (int i = 0; token[i]; i++) {
            if (token[i] >= 'a' && token[i] <= 'z') {
                token[i] = token[i] - 'a' + 'A';
            }
        }
        args[arg_count++] = token;
        token = strtok(NULL, " ");
    }
    
    if (arg_count == 0) return;
    
    if (strcmp(args[0], "HELP") == 0) cmd_help();
    else if (strcmp(args[0], "VER") == 0) cmd_ver();
    else if (strcmp(args[0], "DIR") == 0) cmd_dir();
    else if (strcmp(args[0], "TYPE") == 0) {
        if (arg_count > 1) cmd_type(args[1]);
        else printf("Syntax: TYPE filename\n");
    }
    else if (strcmp(args[0], "CLS") == 0) cmd_cls();
    else if (strcmp(args[0], "MEM") == 0) cmd_mem();
    else if (strcmp(args[0], "REBOOT") == 0) cmd_reboot();
    else if (strcmp(args[0], "COPY") == 0) {
        if (arg_count > 2) cmd_copy(args[1], args[2]);
        else printf("Syntax: COPY source destination\n");
    }
    else if (strcmp(args[0], "DEL") == 0 || strcmp(args[0], "ERASE") == 0) {
        if (arg_count > 1) cmd_del(args[1]);
        else printf("Syntax: DEL filename\n");
    }
    else if (strcmp(args[0], "MOVE") == 0) {
        if (arg_count > 2) cmd_move(args[1], args[2]);
        else printf("Syntax: MOVE source destination\n");
    }
    else if (strcmp(args[0], "CD") == 0 || strcmp(args[0], "CHDIR") == 0) {
        cmd_cd(arg_count > 1 ? args[1] : NULL);
    }
    else if (strcmp(args[0], "MD") == 0 || strcmp(args[0], "MKDIR") == 0) {
        if (arg_count > 1) cmd_md(args[1]);
        else printf("Syntax: MD directory_name\n");
    }
    else if (strcmp(args[0], "RD") == 0 || strcmp(args[0], "RMDIR") == 0) {
        if (arg_count > 1) cmd_rd(args[1]);
        else printf("Syntax: RD directory_name\n");
    }
    else if (strcmp(args[0], "DELTREE") == 0) {
        if (arg_count > 1) cmd_deltree(args[1]);
        else printf("Syntax: DELTREE directory_name\n");
    }
    else if (strcmp(args[0], "PRINT") == 0) {
        if (arg_count > 1) cmd_print(args[1]);
        else printf("Syntax: PRINT filename\n");
    }
    else if (strcmp(args[0], "DATE") == 0) {
        cmd_date(arg_count > 1 ? args[1] : NULL);
    }
    else if (strcmp(args[0], "TIME") == 0) {
        cmd_time(arg_count > 1 ? args[1] : NULL);
    }
    else if (strcmp(args[0], "CON") == 0) {
        if (arg_count > 1) cmd_con(args[1]);
        else cmd_con(NULL);
    }
    else if (strcmp(args[0], "INSTALL") == 0) {
        cmd_install();
    }
    else if (strcmp(args[0], "SAVE") == 0) {
        if (arg_count > 1) cmd_save(args[1]);
        else cmd_save("Manual save");
    }
    else if (strcmp(args[0], "LOAD") == 0) {
        if (arg_count > 1) cmd_load(atoi(args[1]));
        else cmd_load(-1);
    }
    else if (strcmp(args[0], "SAVELIST") == 0) {
        cmd_savelist();
    }
    else if (strcmp(args[0], "SAVEDEL") == 0) {
        if (arg_count > 1) cmd_savedel(atoi(args[1]));
        else printf("Syntax: SAVEDEL slot_number\n");
    }
    else printf("Bad command or file name: %s\n", args[0]);
}

void shell_start() {
    printf("C:\\>");
    
    while (1) {
        char c = keyboard_getchar();
        
        if (c == '\r') {
            putchar('\n');
            command_buffer[command_pos] = 0;
            
            if (command_pos > 0) {
                parse_command(command_buffer);
            }
            
            command_pos = 0;
            printf("C:\\>");
        } else if (c == '\b') {
            if (command_pos > 0) {
                command_pos--;
                putchar('\b');
                putchar(' ');
                putchar('\b');
            }
        } else if (command_pos < MAX_COMMAND_LENGTH - 1) {
            command_buffer[command_pos++] = c;
            putchar(c);
        }
    }
}
