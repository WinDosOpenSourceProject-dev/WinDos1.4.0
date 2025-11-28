#include <stdint.h>

typedef struct {
    uint8_t second;
    uint8_t minute;
    uint8_t hour;
    uint8_t day;
    uint8_t month;
    uint16_t year;
} rtc_time_t;

uint8_t cmos_read(uint8_t address) {
    outb(0x70, address);
    return inb(0x71);
}

void cmos_write(uint8_t address, uint8_t value) {
    outb(0x70, address);
    outb(0x71, value);
}

void rtc_get_time(rtc_time_t* time) {
    time->second = cmos_read(0x00);
    time->minute = cmos_read(0x02);
    time->hour = cmos_read(0x04);
    time->day = cmos_read(0x07);
    time->month = cmos_read(0x08);
    time->year = cmos_read(0x09) + 2000;
    
    time->second = (time->second & 0x0F) + ((time->second / 16) * 10);
    time->minute = (time->minute & 0x0F) + ((time->minute / 16) * 10);
    time->hour = (time->hour & 0x0F) + ((time->hour / 16) * 10);
}

void rtc_set_time(rtc_time_t* time) {
}
