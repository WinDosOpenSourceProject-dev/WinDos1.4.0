#include <stddef.h>

size_t strlen(const char* str) {
    size_t len = 0;
    while (str[len]) len++;
    return len;
}

char* strcpy(char* dest, const char* src) {
    char* ptr = dest;
    while (*src) {
        *ptr++ = *src++;
    }
    *ptr = '\0';
    return dest;
}

char* strcat(char* dest, const char* src) {
    char* ptr = dest + strlen(dest);
    while (*src) {
        *ptr++ = *src++;
    }
    *ptr = '\0';
    return dest;
}

int strcmp(const char* str1, const char* str2) {
    while (*str1 && (*str1 == *str2)) {
        str1++;
        str2++;
    }
    return *(unsigned char*)str1 - *(unsigned char*)str2;
}

char* strncpy(char* dest, const char* src, size_t n) {
    char* ptr = dest;
    while (n-- && *src) {
        *ptr++ = *src++;
    }
    while (n--) {
        *ptr++ = '\0';
    }
    return dest;
}

int strncmp(const char* str1, const char* str2, size_t n) {
    while (n-- && *str1 && (*str1 == *str2)) {
        str1++;
        str2++;
    }
    if (n == (size_t)-1) return 0;
    return *(unsigned char*)str1 - *(unsigned char*)str2;
}

void* memcpy(void* dest, const void* src, size_t n) {
    char* d = (char*)dest;
    const char* s = (const char*)src;
    while (n--) {
        *d++ = *s++;
    }
    return dest;
}

void* memset(void* ptr, int value, size_t n) {
    char* p = (char*)ptr;
    while (n--) {
        *p++ = (char)value;
    }
    return ptr;
}

char* strchr(const char* str, int c) {
    while (*str) {
        if (*str == c) return (char*)str;
        str++;
    }
    return NULL;
}

char* strrchr(const char* str, int c) {
    const char* last = NULL;
    while (*str) {
        if (*str == c) last = str;
        str++;
    }
    return (char*)last;
}

char* strtok(char* str, const char* delim) {
    static char* saved_ptr = NULL;
    
    if (str) saved_ptr = str;
    if (!saved_ptr) return NULL;
    
    char* start = saved_ptr;
    
    while (*saved_ptr && strchr(delim, *saved_ptr)) {
        saved_ptr++;
    }
    
    if (!*saved_ptr) {
        saved_ptr = NULL;
        return NULL;
    }
    
    start = saved_ptr;
    
    while (*saved_ptr && !strchr(delim, *saved_ptr)) {
        saved_ptr++;
    }
    
    if (*saved_ptr) {
        *saved_ptr = '\0';
        saved_ptr++;
    } else {
        saved_ptr = NULL;
    }
    
    return start;
}
