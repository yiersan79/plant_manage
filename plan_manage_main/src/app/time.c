/*
 * time.c - 时间处理
 */

#include "gpio.h"

#include "include/time.h"
#include "include/config.h"


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

void ds1302_byte_write(uint8_t data)
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

uint8_t ds1302_byte_read(void)
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




uint8_t ds1302_single_read(uint8_t addr)
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

void ds1302_set_time(time_info time)
{
    ds1302_single_write(7, 0x00);
    ds1302_single_write(0, time.sec % 10 + (time.sec / 10 << 4));
    ds1302_single_write(1, time.minute % 10 + (time.minute / 10 << 4));
    ds1302_single_write(2, time.hour % 10 + (time.hour / 10 << 4));
    ds1302_single_write(3, time.day % 10 + (time.day / 10 << 4));
    ds1302_single_write(4, time.month % 10 + (time.month / 10 << 4));
    ds1302_single_write(5, time.week);
    ds1302_single_write(6, (time.year - 2000) % 10 + ((time.year - 2000) / 10 << 4));
    return;
}

void ds1302_read_time(time_info *time)
{
    uint8_t rval;

    rval = ds1302_single_read(0);
    (*time).sec = (rval & 0x0f) + ((rval & 0x70) >> 4) * 10;
    rval = ds1302_single_read(1);
    (*time).minute = (rval & 0x0f) + ((rval & 0x70) >> 4) * 10;
    rval = ds1302_single_read(2);
    (*time).hour = (rval & 0x0f) + ((rval & 0x30) >> 4) * 10;
    rval = ds1302_single_read(3);
    (*time).day = (rval & 0x0f) + ((rval & 0x10) >> 4) * 10;
    rval = ds1302_single_read(4);
    (*time).month = (rval & 0x0f) + ((rval & 0x10) >> 4) * 10;
    rval = ds1302_single_read(5);
    (*time).week = rval & 0x07;
    rval = ds1302_single_read(6);
    (*time).year = (rval & 0x0f) + ((rval & 0xf0) >> 4) * 10 + 2000;

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
