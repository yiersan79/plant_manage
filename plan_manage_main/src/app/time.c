/*
 * time.c - 时间处理
 */

#include "include/time.h"



uint8_t is_leapyear(uint16_t year)
{
    return (year % 4 == 0 && year % 100 != 0) || year % 400 == 0;
}

uint8_t get_month_days(uint16_t year, uint8_t month)
{
    switch (month)
    {
    case 4:
    case 6:
    case 9:
    case 11:
        return 30;
    case 2:
        if (is_leapyear(year))
            return 29;
        else
            return 28;
    default:
        return 31;
    }
}
