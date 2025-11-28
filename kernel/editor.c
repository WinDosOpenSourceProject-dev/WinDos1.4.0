#include "string.h"
#include "fat16.h"
#include "drivers/keyboard.h"

#define EDITOR_BUFFER_SIZE 8192

typedef struct {
    char content[EDITOR_BUFFER_SIZE];
    int length;
    int cursor_pos;
    int saved;
} editor_state_t;

editor_state_t editor;

void editor_init() {
    editor.length = 0;
    editor.cursor_pos = 0;
    editor.saved = 0;
}

int editor_load_file(const char* filename) {
    editor_init();
    
    int size = fat16_read_file(filename, editor.content, EDITOR_BUFFER_SIZE - 1);
    if (size > 0) {
        editor.length = size;
        editor.content[editor.length] = '\0';
        return 0;
    }
    
    return -1;
}

int editor_save_file(const char* filename) {
    if (fat16_create_file(filename, editor.content, editor.length) == 0) {
        editor.saved = 1;
        return 0;
    }
    return -1;
}

void editor_insert_char(char c) {
    if (editor.length < EDITOR_BUFFER_SIZE - 1) {
        for (int i = editor.length; i > editor.cursor_pos; i--) {
            editor.content[i] = editor.content[i - 1];
        }
        
        editor.content[editor.cursor_pos] = c;
        editor.cursor_pos++;
        editor.length++;
        editor.content[editor.length] = '\0';
        editor.saved = 0;
    }
}

void editor_delete_char() {
    if (editor.cursor_pos > 0 && editor.length > 0) {
        for (int i = editor.cursor_pos - 1; i < editor.length; i++) {
            editor.content[i] = editor.content[i + 1];
        }
        
        editor.cursor_pos--;
        editor.length--;
        editor.saved = 0;
    }
}

void cmd_con_advanced(const char* filename) {
    if (!filename) {
        printf("Syntax: CON filename\n");
        printf("Full-screen text editor\n");
        printf("Ctrl+S: Save and exit\n");
        printf("Ctrl+C: Cancel\n");
        printf("Ctrl+V: Paste mode\n");
        return;
    }
    
    printf("WinDos Editor - Editing: %s\n", filename);
    
    if (editor_load_file(filename) == 0) {
        printf("Loaded existing file (%d bytes)\n", editor.length);
    } else {
        printf("Creating new file\n");
        editor_init();
    }
    
    int editing = 1;
    printf("Entering editor mode...\n");
    
    while (editing) {
        printf("\n--- Editor: (Ctrl+S:Save, Ctrl+C:Cancel, Ctrl+V:Paste) ---\n");
        printf("%s\n", editor.content);
        printf("--- [Pos:%d/%d] ---\n", editor.cursor_pos, editor.length);
        printf("Command: ");
        
        char c = keyboard_getchar();
        
        switch (c) {
            case 0x13:
                printf("SAVE\n");
                if (editor_save_file(filename) == 0) {
                    printf("File saved successfully: %s (%d bytes)\n", filename, editor.length);
                } else {
                    printf("Error saving file!\n");
                }
                editing = 0;
                break;
                
            case 0x03:
                printf("CANCEL\n");
                if (!editor.saved) {
                    printf("Unsaved changes will be lost. Confirm (Y/N)? ");
                    char confirm = keyboard_getchar();
                    putchar(confirm);
                    if (confirm == 'Y' || confirm == 'y') {
                        printf("\nEdit cancelled\n");
                        editing = 0;
                    } else {
                        printf("\nContinue editing...\n");
                    }
                } else {
                    editing = 0;
                }
                break;
                
            case 0x16:
                printf("PASTE\n");
                printf("Enter text to paste (press ENTER twice to finish):\n");
                
                char paste_buffer[1024];
                int paste_pos = 0;
                int pasting = 1;
                
                while (pasting && paste_pos < sizeof(paste_buffer) - 1) {
                    char paste_char = keyboard_getchar();
                    
                    if (paste_char == '\r') {
                        char next_char = keyboard_getchar();
                        if (next_char == '\r') {
                            pasting = 0;
                        } else {
                            editor_insert_char('\n');
                            if (next_char >= 32 && next_char <= 126) {
                                editor_insert_char(next_char);
                            }
                        }
                    }
                    else if (paste_char == '\b') {
                        if (paste_pos > 0) {
                            paste_pos--;
                            editor_delete_char();
                        }
                    }
                    else if (paste_char >= 32 && paste_char <= 126) {
                        editor_insert_char(paste_char);
                        paste_buffer[paste_pos++] = paste_char;
                    }
                }
                printf("\nPaste completed (%d characters added)\n", paste_pos);
                break;
                
            case '\b':
                editor_delete_char();
                break;
                
            case '\r':
                editor_insert_char('\n');
                break;
                
            default:
                if (c >= 32 && c <= 126) {
                    editor_insert_char(c);
                }
                break;
        }
    }
    
    printf("Exiting editor. Returning to command prompt.\n");
}
