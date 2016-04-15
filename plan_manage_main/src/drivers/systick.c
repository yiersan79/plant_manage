/******************** (C) COPYRIGHT 2011 蓝宙电子工作室 ********************
* 文件名       ： adc.c
* 描述         ：工程模版实验
*
* 实验平台     ：landzo电子开发版
* 库版本       ：
* 嵌入系统     ：
*
* 作者         ：landzo 蓝电子
* 淘宝店       ：http://landzo.taobao.com/

**********************************************************************************/
#include "systick.h"

// 创建系统滴答计数变量
uint32_t systicks;
uint32_t timecount = 0;
extern uint8_t time[3];                    //记录时间的数组
//===========================================================================
//函数名称：systick_init
//函数返回：无
//参数说明：core_clk_hz：内核时钟频率
//      ticks_per_sec：滴答频率，以Hz为单位。
//功能概要：初始化ARM Cortex-M处理器内核的SysTick模块作为系统滴答时钟。
//===========================================================================
void systick_init(uint32_t core_clk_hz, uint32_t ticks_per_sec)
{
	// 初始化系统滴答计数值
    systicks = 0; 
    // 关闭SYSTICK
    SYST_CSR = 0;
    // 清除当前值计数器  
    SYST_CVR = 0;
    // 设定倒计时计数值
    SYST_RVR = core_clk_hz / ticks_per_sec - 1;
    // 设定 SysTick优先级
    SCB_SHPR3 |= SCB_SHPR3_PRI_15(1);  
    // 使用内核时钟，倒计时到0时产生SYSTICK中断，使能SYSTICK
    SYST_CSR = 7; 
}


//===========================================================================
//函数名称：SecAdd1
//函数返回：无
//参数说明：*p:计数变量的首地址 
//功能概要：以秒为最小单位递增时,分,秒缓冲区的值(00:00:00-23:59:59)
//===========================================================================
void SecAdd1(uint8_t *p)
{
    *(p+2) += 1;             //秒加1
    if (*(p+2) >= 60)        //秒溢出
    {
        *(p+2)  = 0;         //清秒
        *(p+1) += 1;         //分加1
        if (*(p+1) >= 60)    //分溢出
        {
            *(p+1)  = 0;     //清时
            *p += 1;         //时加1
            if (*p >= 24)    //时溢出
                *p = 0;      //清时
        }
    }      
}
//===========================================================================
//函数名称：systick_trim
//函数返回：无
//参数说明：flag: 0表示慢，1表示快
//       trim_sec：每一分钟的秒数之差      
//功能概要：校准systick,如果每分钟慢一秒，SYST_RVR就减少48M/6000=8000
//                 如果每分钟快一秒，SYST_RVR就增加48M/6000=8000
//===========================================================================
void systick_trim(uint32_t core_clk_hz,uint32_t ticks_per_sec, uint32_t flag, uint32_t trim_sec)
{
	  // 初始化系统滴答计数值
    systicks = 0; 
    // 关闭SYSTICK
    SYST_CSR = 0;
    // 清除当前值计数器  
    SYST_CVR = 0;
    // 设定倒计时计数值
    if(0 == flag)
    {
    	SYST_RVR = core_clk_hz / ticks_per_sec - 1 - 8000*trim_sec;
    }
    else
    {
    	SYST_RVR = core_clk_hz / ticks_per_sec - 1 + 8000*trim_sec;	
    }
    // 设定 SysTick优先级
    SCB_SHPR3 |= SCB_SHPR3_PRI_15(1);  
    // 使用内核时钟，倒计时到0时产生SYSTICK中断，使能SYSTICK
    SYST_CSR = 7; 
}

//===========================================================================
//函数名称：systick_isr
//函数返回：无
//参数说明：无
//功能概要：SysTick定时器中断服务例程，更新系统滴答计数变量值。
//===========================================================================
void systick_isr(void)
{
    if (systicks >= 0xFFFFFFFF)
    {
        systicks = 0;
    }
    else 
    {
        systicks++;
    }
	timecount++;
	if(timecount > 99)
	{
		timecount = 0;
	    SecAdd1(time);                //1s到,递增时,分,秒缓冲区的值
	}
}
