/*
 * time.h - 时间处理
 */

#include <stdint.h>

typedef struct time_info_
{
    uint16_t year;
    uint8_t month;
    uint8_t week;
    uint8_t day;
    uint8_t hour;
    uint8_t minute;
    uint8_t sec;
} time_info;



uint8_t is_leapyear(uint16_t year);

uint8_t get_month_days(uint16_t year, uint8_t month);

void ds1302_init(void);

void ds1302_set_time(time_info time);

void ds1302_read_time(time_info *time);
