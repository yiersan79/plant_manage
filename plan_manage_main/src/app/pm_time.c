/*
 * pm_time.c - 时间处理
 */

#include "gpio.h"

#include "include/pm_time.h"
#include "include/config.h"


// 被维护的系统时间
static calendar_info system_time;

#define DAY_IN_YEAR(nyear)   (is_leapyear(nyear) ? 366 : 365)

static const uint8_t day_leap[] = {
    0, 31, 29, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31
};

static const uint8_t day_noleap[] = {
    0, 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31
};



/*
 * 静态函数声明
 */
static void ds1302_byte_write(uint8_t data);
static uint8_t ds1302_byte_read(void);
static uint8_t ds1302_single_read(uint8_t addr);
void ds1302_single_write(uint8_t addr, uint8_t data);
static int16_t ymd_to_wday(int16_t year, int16_t month, int16_t mday);


/**
 * ds1302_init() - ds1302的初始化
 *
 * 包括io的初始化以及当ds1302掉电时对其时间值设置初始值
 */
void ds1302_init(void)
{
    //gpio_Interrupt_init(DS1302_CE_PINX, GPO, GPI_DISAB);
    //gpio_Interrupt_init(DS1302_CLK_PINX, GPO, GPI_DISAB);
    gpio_init(DS1302_CE_PINX, 1, 0);
    gpio_init(DS1302_CLK_PINX, 1, 0);
    gpio_Interrupt_init(DS1302_IO_PINX, GPI_UP_PF, GPI_DISAB);

    // 使能写
    ds1302_single_write(7, 0x00);
    // 当ds1302掉电时设置时间初始值
    if ((ds1302_single_read(0) & 0x80) == 1)
    {
        calendar_info sys_cal = { 0, 1, 2, 3, 4, 2012, 0, 1 };
        ds1302_set_time(&sys_cal);
    }
    return;
}


/**
 * is_leapyear() - 判断是否为闰年
 * @year: 要判断的年份
 *
 * 是闰年返回1，不是闰年返回0
 */
uint8_t is_leapyear(uint16_t year)
{
    return (year % 4 == 0 && year % 100 != 0) || year % 400 == 0;
}

/**
 * get_month_days() - 得到给定年份和月份时对应月份的天数
 * @year: 给定年份
 * @month: 给定月份
 *
 * 返回值为对应的月份的天数
 */
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

/**
 * ds1302_byte_write() - 向ds1302写入一个字节
 * @data: 要写入的字节
 */
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

/**
 * ds1302_byte_read() - 从ds1302读取一个字节
 *
 * 返回读取到的字节
 */
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



/**
 * ds1302_single_read() - ds1302在单字节模式下读取一个地址上的数据
 * @addr: 要读取的地址
 *
 * 返回读取的到的字节值
 */
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

/**
 * ds1302_single_write() - 在单字节模式下向ds1302的一个地址上写入一个字节的数据
 * @addr: 要写入数据的地址
 * @data: 要写入的数据
 */
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

/**
 * ds1302_set_time() - 向ds1302中写入时间
 * @cal: 要写入的时间值地址
 */
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

/**
 * ds1302_read_time() - 从ds1302中读取时间
 * @cal: 读取的时间值被存放的地址
 */
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
    cal->mday = (rval & 0x0f) + ((rval & 0x30) >> 4) * 10;
    rval = ds1302_single_read(4);
    cal->month = (rval & 0x0f) + ((rval & 0x10) >> 4) * 10;
    rval = ds1302_single_read(5);
    cal->wday = rval & 0x07;
    rval = ds1302_single_read(6);
    cal->year = (rval & 0x0f) + ((rval & 0xf0) >> 4) * 10 + 2000;

    return;
}

/**
 * maintain_system_time() - 设置系统时间变量
 *
 * 这个函数需要按所需的时间精度来定时调用
 */
void maintain_system_time(void)
{
    enter_critical();
    ds1302_read_time(&system_time);
    exit_critical();
    return;
}

/**
 * get_system_time() - 返回系统时间
 */
calendar_info get_system_time(void)
{
    return system_time;
}




/**
 * calendar_to_sec() - 分解时间到日历时间的转换
 * @cal: 被转换的分解时间
 *
 * 返回的是日历时间，即从某一个时间点到当前转换时间所经过的秒数
 */
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

/**
 * sec_to_calendar() - 日历时间到分解时间的转换
 * @cal: 被转换的日历时间
 *
 * 返回的是分解时间，即以年月日时分秒形式表示的时间
 */
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

/**
 * ymd_to_wday() - 从年份、月份、天数这三个数据得到对应在一个星期中的天数
 * @year: 年份
 * @month: 月份
 * @mday: 当前月中已经过的天数
 *
 * 返回对应的一个星期中的天数
 */
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
