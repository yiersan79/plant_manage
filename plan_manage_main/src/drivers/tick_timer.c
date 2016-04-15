//===========================================================================
//文件名称：tick_timer.c
//功能概要：滴答定时器构件源文件
//版权所有：苏州大学飞思卡尔嵌入式中心(sumcu.suda.edu.cn)
//更新记录：2013-01-29   V1.0
//===========================================================================

#include "tick_timer.h"

//===========================================================================
//函数名称：tick_timer_start
//函数返回：无
//参数说明：timer_ptr：软件定时器指针
//          time_off：设定延时时间长度，以滴答为单位
//功能概要：启动软件定时器并指定延时时间长度
//===========================================================================
void tick_timer_start(TICKS_TIMER_PTR timer_ptr, uint_32 time_off)
{
    timer_ptr->FLAG = 1;
    timer_ptr->LAST_TICK = systicks;
    timer_ptr->TIME_OFF = time_off;
}

//===========================================================================
//函数名称：tick_timer_is_out
//函数返回：定时器超时标识
//          TRUE（1） - 定时时间到； FALSE（0） - 定时时间未到
//参数说明：timer_ptr：软件定时器指针
//功能概要：查看延时时刻是否到来
//===========================================================================
uint_8 tick_timer_is_out(TICKS_TIMER_PTR timer_ptr)
{
    uint_8 bRet = 0;
    uint_32 tmp = timer_ptr->LAST_TICK + timer_ptr->TIME_OFF;
    
    if (timer_ptr->FLAG)
    {
        if(timer_ptr->LAST_TICK > systicks) //TICKS已回头
        {
            tmp = 0xFFFFFFFF - timer_ptr->LAST_TICK + systicks;
            if(tmp >= timer_ptr->TIME_OFF)
            {
                bRet = 1;
            }
        }
        else if(tmp <= systicks)
        {
		        bRet = 1;
		    }
    } // end_if
    
    return bRet;
}

//===========================================================================
//函数名称：tick_timer_stop
//函数返回：无
//参数说明：timer_ptr：软件定时器指针
//功能概要：停用软件定时器
//===========================================================================
void tick_timer_stop(TICKS_TIMER_PTR timer_ptr)
{
    timer_ptr->FLAG = 0; 
}
