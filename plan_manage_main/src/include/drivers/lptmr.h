/******************** (C) COPYRIGHT 2011 蓝宙电子工作室 ********************
* 文件名       ： lptmr.h
* 描述         ：工程模版实验
*
* 实验平台     ：landzo电子开发版
* 库版本       ：
* 嵌入系统     ：
*
* 作者         ：landzo 蓝电子
* 淘宝店       ：http://landzo.taobao.com/

**********************************************************************************/
#ifndef __lptmr_H__
#define __lptmr_H__


#include "include.h"

//中断号
#define LPTMRLPTMR_irq 28  

/**
 *  @brief LPTMR脉冲计数输入管脚选项
 */
typedef enum
{
    //只有1、2管脚，并没有0、3管脚
    LPT0_ALT1 = 1,      // PTA19
    LPT0_ALT2 = 2       // PTC5
} LPT0_ALTn;

/**
 *  @brief LPTMR脉冲计数触发方式
 */
typedef enum LPT_CFG
{
    LPT_Rising  = 0,    //上升沿触发
    LPT_Falling = 1     //下降沿触发
} LPT_CFG;

#define LPTMR_Flag_Clear()  (LPTMR0_CSR |= LPTMR_CSR_TCF_MASK)         //清除LPT比较标志位


/*          用于延时         */
void     lptmr_delay_ms(uint16_t ms);      //延时(ms)
void     lptmr_delay_us(uint16_t us);      //延时(us)

/*       用于定时           */
void     lptmr_timing_ms(uint16_t ms);     //定时(ms)
void     lptmr_timing_us(uint16_t ms);     //定时(us)

/*        用于计时           */
void     lptmr_time_start_ms(void);      //开始计时(ms)
uint32_t   lptmr_time_get_ms(void);        //获取计时时间 

void     lptmr_time_start_us(void);      //开始计时(ns)
uint32_t   lptmr_time_get_us(void);        //获取计时时间 

void     lptmr_time_close();             //关闭计时器


/*       用于脉冲计数        */
void     lptmr_counter_init (LPT0_ALTn, uint16_t count, LPT_CFG);   //计数器初始化设置
uint16_t   lptmr_pulse_get   (void);                               //获取计数值
void     lptmr_pulse_clean (void);                               //清空计数值


                                   //中断复位函数，仅供参考（需用户自行实现）

//============================================================================
//函数名称：lptmr_internal_ref_init
//函数返回：无
//参数说明：内部参考时钟初始化
//功能概要：采用内部参考时钟（PSC=0X0）
//         内部参考时钟有两个时钟源:(1)MCG_C2[IRCS]=0,使用慢速内部时钟(32kHz)
//                                 (2)MCG_C2[IRCS]=1,使用快速内部时钟(2Mhz)   
//         本例采用的是快速时钟源，周期= compare_value/ClkBus/prescale=4秒
//         prescale = 2^(8+1)=512,ClkBus=2Mhz,compare_value=15625
//============================================================================
void lptmr_internal_ref_init();

//============================================================================
//函数名称：enable_lptmr_int
//函数返回：无
//参数说明：无
//功能概要：开启LPT定时器中断
//============================================================================
void enable_lptmr_int();

//============================================================================
//函数名称：disable_lptmr_int
//函数返回：无
//参数说明：无
//功能概要：关闭LPT定时器中断
//============================================================================
void disable_lptmr_int();

#endif /* __LPTMR_H__ */
