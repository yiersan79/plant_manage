/******************** (C) COPYRIGHT 2011 蓝宙电子工作室 ********************
* 文件名       ： adc.H
* 描述         ：工程模版实验
*
* 实验平台     ：landzo电子开发版
* 库版本       ：
* 嵌入系统     ：
*
* 作者         ：landzo 蓝电子
* 淘宝店       ：http://landzo.taobao.com/

**********************************************************************************/

#ifndef _ADC_H_
#define _ADC_H_

#include "include.h"

typedef enum
{
    // ---------------------------------ADC0-------------------------
    ADC0_DP0 = 0,   // PTE20
    ADC0_SE0 = 0,

    ADC0_DP1 = 1,   // PTE16
    ADC0_SE1 = 1,

    ADC0_DP2 = 2,   // PTE18
    ADC0_SE2 = 2,

    ADC0_DP3 = 3,   // PTE22
    ADC0_SE3 = 3,

    ADC0_DM0 = 4,   // PTE21
    ADC0_SE4a= 4,

    ADC0_DM1 = 5,   // PTE17
    ADC0_SE5a= 5,

    ADC0_DM2 = 6,   // PTE19
    ADC0_SE6a= 6,

    ADC0_DM3 = 7,   // PTE23
    ADC0_SE7a= 7,

    ADC0_SE4b= 4,   // PTE29     不支持软件ADC，传递进软件触发ADC，会当作 a通道处理

    ADC0_SE5b= 5,   // PTD1      不支持软件ADC，传递进软件触发ADC，会当作 a通道处理

    ADC0_SE6b= 6,   // PTD5      不支持软件ADC，传递进软件触发ADC，会当作 a通道处理

    ADC0_SE7b= 7,   // PTD6      不支持软件ADC，传递进软件触发ADC，会当作 a通道处理

    ADC0_SE8,       // PTB0

    ADC0_SE9,       // PTB1

    ADC0_RES0,      // 保留

    ADC0_SE11,      // PTC2

    ADC0_SE12,      // PTB2

    ADC0_SE13,      // PTB3

    ADC0_SE14,      // PTC0

    ADC0_SE15,      // PTC1

    ADC0_RES1,      // 保留

    ADC0_RES2,      // 保留

    ADC0_RES3,      // 保留

    ADC0_RES4,      // 保留

    ADC0_RES5,      // 保留

    ADC0_RES6,      // 保留

    ADC0_RES7,      // 保留

    ADC0_SE23,      // PTE30

    DAC0_OUT = ADC0_SE23, // PTE30  DAC0输出 ,传入 ADC函数会当作 ADC0_SE23  处理

    ADC0_RES8,      // 保留

    ADC0_RES9,      // 保留

    Temp0_Sensor,   // Temperature Sensor,内部温度测量，可用ADC函数
    Bandgap0,       // 温度补偿结构带隙基准源   不支持ADC
    ADC0_RES10,     // 保留
    VREFH0,         // 参考高电压,可用ADC函数 ,结果恒为 2^n-1
    VREFL0,         // 参考低电压,可用ADC函数 ,结果恒为 0
    Module0_Dis,    // 不支持 ADC

} ADCn_Ch_e;

typedef enum  //ADC模块
{
    ADC0,
    ADC1
} ADCn_e;

//精度位数
typedef enum ADC_nbit
{
    ADC_8bit   = 0x00,
    ADC_10bit  = 0x02,
    ADC_12bit  = 0x01,
    ADC_16bit  = 0x03
} ADC_nbit;
enum HardwareAverage
{
	sample4 = 0,
	sample8 = 1,
	sample16 = 2,
	sample32 = 3,
};

//外部函数接口声明
void      adc_init(ADCn_Ch_e);              	                //ADC初始化
uint16_t  adc_once (ADCn_Ch_e, ADC_nbit);    	                //采集一次一路模拟量的AD值
uint16_t adc_ave(ADCn_Ch_e adcn_ch, ADC_nbit bit, uint8_t N);   //均值滤波
uint16_t  adc_sum(ADCn_Ch_e adcn_ch, ADC_nbit bit, u8 N);        //采样集合
void adc_start(ADCn_Ch_e adcn_ch, ADC_nbit bit) ;
void adc_stop(ADCn_e);                                          //停止ADC转换

#endif
