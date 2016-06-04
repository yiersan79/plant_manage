/*
 * simulat_timer.h - 实现一个模拟定时器
 */

#include "include.h"

#include "include/config.h"
#include "include/key.h"

#include "include/simulat_timer.h"

static volatile st_register st_r[SIMULAT_TIMER_NUM];



/**
 * st_init() - 初始化实现模拟定时器所需的pit定时器
 *
 * 基础定时为1ms
 */
void st_base_init(void)
{
    uint32_t cnt;

    cnt = bus_clk_khz * 1;
    pit_init(SIMULAT_TIMER_PITX, cnt);  // 1ms硬件定时中断周期
    return;
}


/**
 * st_init() - 初始化一个给定编号的模拟定时器
 * @n: 定时器编号
 * @st_m: 定时模式
 * @cmr_v: 定时器比较值，在非比较模式可以为任意值
 *
 * 这个函数返回0表示正常完成了初始化任务，返回~0表示定时器已打开或不存在
 */
uint8_t st_init(uint8_t n, st_mode st_m, uint32_t cmr_v)
{
    if (st_r[n].flag.ten == 1 || n >= SIMULAT_TIMER_NUM)
    {
        return ~0;
    }
    st_r[n].flag.ten = 1;
    switch(st_m)
    {
    case COMPARE:
        st_r[n].flag.tfc = 1;
        st_r[n].cmr = cmr_v;
        break;
    case NO_COMPARE:
        st_r[n].flag.tfc = 0;
        break;
    default:
        break;
    }
    return 0;
}


/**
 * st_close() - 关闭一个给定编号的模拟定时器
 * @n: 定时器编号
 */
void st_close(uint8_t n)
{
    st_r[n].flag.ten = 0;
    return;
}

/**
 * st_tcf() - 查询tcf位返回
 * @n: 定时器编号
 *
 * 这个函数在没有过比较值的时候返回0，已过比较值得时候返回1，对于不存在或未打开
 */
uint8_t st_tcf(uint8_t n)
{
    uint8_t temp;

    if (st_r[n].flag.tcf == 1)
    {
        st_r[n].flag.tcf = 0;
        temp = 1;
    }
    else
    {
        temp = 0;
    }
    return temp;
}

/**
 * st_tov() - 查询tov位返回
 * @n: 定时器编号
 *
 * 这个函数在没有溢出时返回0，没有溢出时返回1，对于不存在或未打开
 */
uint8_t st_tov(uint8_t n)
{
    uint8_t temp;
    if (st_r[n].flag.tov == 1)
    {
        st_r[n].flag.tov = 0;
        temp = 1;
    }
    else
    {
        temp = 0;
    }
    return temp;
}

/**
 * st_count() - 查询当前模拟定时值
 * @n: 定时器编号
 *
 * 对于不存在或未打开
 */
uint32_t st_count(uint8_t n)
{
    return st_r[n].count;
}


/**
 * st_base() -定时调用以模拟N路定时器
 *
 * 定时的最小间隔等于这个函数被调用的周期
 */
void st_base(void)
{
    uint8_t i;

    for (i = 0; i < SIMULAT_TIMER_NUM; i++)
    {
        if (st_r[i].flag.ten == 1)          // 使能
        {
            if (++st_r[i].count == 0)       // 溢出
            {
                st_r[i].flag.tov = 1;
            } // 溢出
            if (st_r[i].flag.tfc == 1)      // 比较
            {
                if (st_r[i].count == st_r[i].cmr)
                {
                    st_r[i].count = 0;
                    st_r[i].flag.tcf = 1;
                }
            } // 比较
        } // 使能
    }
    
    if (st_tcf(0) == 1)
    {
        key_detect();
    }
    
    return;
}
