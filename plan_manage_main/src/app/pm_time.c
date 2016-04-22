/*
 * pm_time.c - 时间处理
 */

#include "gpio.h"

#include "include/pm_time.h"
#include "include/config.h"



static calendar_info system_time;

enum { START_YEAR = 2000, SEC_IN_DAY = 24 * 60 * 60};
#define DAY_IN_YEAR(nyear)   (is_leapyear(nyear) ? 366 : 365)

static const uint8_t day_leap[] = {
    0, 31, 29, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31
};

static const uint8_t day_noleap[] = {
    0, 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31
};



static void ds1302_byte_write(uint8_t data);
static uint8_t ds1302_byte_read(void);
static uint8_t ds1302_single_read(uint8_t addr);
static uint8_t ds1302_single_read(uint8_t addr);
static int16_t ymd_to_wday(int16_t year, int16_t month, int16_t mday);






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

static void ds1302_byte_write(uint8_t data)
{
    gpio_init(DS1302_IO_PINX, 1, 0);
    for (uint8_t mask = 0x01; mask != 0; mask <<= 1)
    {
        gpio_set(DS1302_CLK_PINX, 0);
        if (data & mask)
        {
            gpio_set(DS1302_IO_PINX, 1);
        }
        else
        {
            gpio_set(DS1302_IO_PINX, 0);
        }
        // tdc,200ns,数据建立时间,tcl,1000ns,sclk低电平保持时间
        for (uint8_t i = 0; i < 50; i++);
        gpio_set(DS1302_CLK_PINX, 1);
        // tcdh,280ns,数据采集时间,tch,1000ns,sclk高电平保持时间
        for (uint8_t i = 0; i < 50; i++);
    }
    return;
}

static uint8_t ds1302_byte_read(void)
{
    uint8_t data = 0;

    //gpio_init(DS1302_IO_PINX, 0, 1);
    gpio_Interrupt_init(DS1302_IO_PINX, GPI_UP_PF, GPI_DISAB);
    gpio_set(DS1302_CLK_PINX, 1);
    // tccz,280ns,sclk到高阻态
    for (uint8_t i = 0; i < 50; i++);
    for (uint8_t mask = 0x01; mask != 0; mask <<= 1)
    {
        gpio_set(DS1302_CLK_PINX, 0);   // 产生下降沿
        // tcdd,800ns,数据输出延迟
        for (uint8_t i = 0; i < 50; i++);
        for (uint8_t i = 0; i < 50; i++);
        if (gpio_get(DS1302_IO_PINX))
        {
            data |= mask;
        }
        gpio_set(DS1302_CLK_PINX, 1);
        // tccz,280ns,sclk到高阻态
        for (uint8_t i = 0; i < 50; i++);
    }
    return data;
}




static uint8_t ds1302_single_read(uint8_t addr)
{
    uint8_t cmd = 0,
            data = 0;

    cmd = (1 << 7) | (addr << 1) | 1;
    /*
     * 初始化ce和clk状态
     */
    gpio_set(DS1302_CE_PINX, 0);
    for (uint8_t i = 0; i < 50; i++);
    gpio_set(DS1302_CLK_PINX, 0);
    for (uint8_t i = 0; i < 50; i++);

    gpio_set(DS1302_CE_PINX, 1);
    // tcc,ce到时钟建立时间，4us
    for (uint8_t i = 0; i < 200; i++);
    ds1302_byte_write(cmd);
    data = ds1302_byte_read();
    gpio_set(DS1302_CE_PINX, 0);
    // tcdz,ce到高阻态时间，280ns
    for (uint8_t i = 0; i < 20; i++);
    return data;
}

void ds1302_single_write(uint8_t addr, uint8_t data)
{
    uint8_t cmd = 0;

    cmd = (1 << 7) | (addr << 1);
    /*
     * 初始化ce和clk状态
     */
    gpio_set(DS1302_CE_PINX, 0);
    for (uint8_t i = 0; i < 50; i++);
    gpio_set(DS1302_CLK_PINX, 0);
    for (uint8_t i = 0; i < 50; i++);

    gpio_set(DS1302_CE_PINX, 1);
    // tcc,ce到时钟建立时间，4us
    for (uint8_t i = 0; i < 200; i++);
    ds1302_byte_write(cmd);
    ds1302_byte_write(data);
    gpio_set(DS1302_CE_PINX, 0);
    return;
}

void ds1302_set_time(calendar_info *cal)
{
    ds1302_single_write(7, 0x00);
    ds1302_single_write(0, cal->sec % 10 + (cal->sec / 10 << 4));
    ds1302_single_write(1, cal->min % 10 + (cal->min / 10 << 4));
    ds1302_single_write(2, cal->hour % 10 + (cal->hour / 10 << 4));
    ds1302_single_write(3, cal->mday % 10 + (cal->mday / 10 << 4));
    ds1302_single_write(4, cal->month % 10 + (cal->month / 10 << 4));
    ds1302_single_write(5, cal->wday);
    ds1302_single_write(6, (cal->year - 2000) % 10 + ((cal->year - 2000) / 10 << 4));
    return;
}

void ds1302_read_time(calendar_info *cal)
{
    uint8_t rval;

    rval = ds1302_single_read(0);
    cal->sec = (rval & 0x0f) + ((rval & 0x70) >> 4) * 10;
    rval = ds1302_single_read(1);
    cal->min = (rval & 0x0f) + ((rval & 0x70) >> 4) * 10;
    rval = ds1302_single_read(2);
    cal->hour = (rval & 0x0f) + ((rval & 0x30) >> 4) * 10;
    rval = ds1302_single_read(3);
    cal->mday = (rval & 0x0f) + ((rval & 0x10) >> 4) * 10;
    rval = ds1302_single_read(4);
    cal->month = (rval & 0x0f) + ((rval & 0x10) >> 4) * 10;
    rval = ds1302_single_read(5);
    cal->wday = rval & 0x07;
    rval = ds1302_single_read(6);
    cal->year = (rval & 0x0f) + ((rval & 0xf0) >> 4) * 10 + 2000;

    return;
}

void ds1302_init(void)
{
    //gpio_Interrupt_init(DS1302_CE_PINX, GPO, GPI_DISAB);
    //gpio_Interrupt_init(DS1302_CLK_PINX, GPO, GPI_DISAB);
    gpio_init(DS1302_CE_PINX, 1, 0);
    gpio_init(DS1302_CLK_PINX, 1, 0);
    gpio_Interrupt_init(DS1302_IO_PINX, GPI_UP_PF, GPI_DISAB);
    ds1302_single_write(7, 0x00);
    return;
}



/*
 * 这个函数需要按所需的时间精度来定时调用
 */
void maintain_system_time(void)
{
    enter_critical();
    ds1302_read_time(&system_time);
    exit_critical();
    return;
}

calendar_info get_system_time(void)
{
    return system_time;
}



/*************************************************************/


uint32_t calendar_to_sec(calendar_info *cal)
{
    uint32_t sec = cal->sec;
    uint32_t year = cal->year;
    uint32_t month = cal->month;

    if (year < START_YEAR || year > (START_YEAR + 135))
        return 0;

    sec += (uint32_t)cal->min * 60;
    sec += (uint32_t)cal->hour * 3600;
    sec += (uint32_t)(cal->mday - 1) * SEC_IN_DAY;
    if (is_leapyear(year))
    {
        while (month > 1)
        {
            sec += (uint32_t)day_leap[--month] * SEC_IN_DAY;
        }
    }
    else
    {
        while (month > 1)
        {
            sec += (uint32_t)day_noleap[--month] * SEC_IN_DAY;
        }
    }
    while (year > START_YEAR)
    {
        sec += (uint32_t)DAY_IN_YEAR(--year) * SEC_IN_DAY;
    }
    return sec;
}

calendar_info sec_to_calendar(uint32_t sec)
{
    calendar_info cal;
    uint32_t day, left;

    cal.year = START_YEAR;
    cal.month = 1;
    cal.mday = 1;
    cal.yday = 1;
    cal.wday = ymd_to_wday(START_YEAR, 1, 1);

    day = sec / SEC_IN_DAY;
    left = sec % SEC_IN_DAY;

    cal.wday = (day + cal.wday) % 7;

    cal.hour = left / 3600;
    cal.min = left / 60 % 60;
    cal.sec = left % 60;

    while (day >= DAY_IN_YEAR(cal.year))
    {
        day -= DAY_IN_YEAR(cal.year++);
    }
    cal.yday += day;

    if (is_leapyear(cal.year))
    {
        while (day >= day_leap[cal.month])
        {
            day -= day_leap[cal.month++];
        }
    }
    else
    {
        while (day >= day_noleap[cal.month])
        {
            day -= day_noleap[cal.month++];
        }
    }
    cal.mday += day;

    return cal;
}


static int16_t ymd_to_wday(int16_t year, int16_t month, int16_t mday)
{
    if (is_leapyear(year))
    {
        for (uint8_t i = 1; i < month; i++)
            mday += day_leap[i];
    }
    else
    {
        for (uint8_t i = 1; i < month; i++)
            mday += day_noleap[i];
    }
    return (year + year / 4 - year / 100 + year / 400 + mday) % 7;
}
