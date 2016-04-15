/******************** (C) COPYRIGHT 2011 蓝宙电子工作室 ********************
* 文件名       ： port.h
* 描述         ：工程模版实验
*
* 实验平台     ：landzo电子开发版
* 库版本       ：
* 嵌入系统     ：
*
* 作者         ：landzo 蓝电子
* 淘宝店       ：http://landzo.taobao.com/

**********************************************************************************/

#ifndef __PORT_H__
#define __PORT_H__

#include "include.h"

//得出：PTx = PTxn / 32 ; PTn = PTxn & 31
#define PTX(PTxn)           ((PTxn)>>5)              //PORTX PORTA PORTB PORTC PORTD PORTE
#define PTn(PTxn)           ((PTxn)&0x1f)           //PIN   0~31
#define PORTX_BASE(PTxn)     PORTX[PTX(PTxn)]       //PORT模块的地址
#define GPIOX_BASE(PTxn)     GPIOx[PTX(PTxn)]       //GPIOx模块的地址

/*! 枚举PORT 配置 */
typedef enum
{
    //中断方式和DMA请求方式，两者只能选其中一种（可以不选）
    //中断方式选择
    IRQ_ZERO     = 0x08 << PORT_PCR_IRQC_SHIFT,   //低电平触发
    IRQ_RISING   = 0x09 << PORT_PCR_IRQC_SHIFT,   //上升沿触发
    IRQ_FALLING  = 0x0A << PORT_PCR_IRQC_SHIFT,   //下降沿触发
    IRQ_EITHER   = 0x0B << PORT_PCR_IRQC_SHIFT,   //跳变沿触发
    IRQ_ONE      = 0x0C << PORT_PCR_IRQC_SHIFT,   //高电平触发

    //DMA请求选择
    DMA_RISING   = 0x01 << PORT_PCR_IRQC_SHIFT,   //上升沿触发
    DMA_FALLING  = 0x02 << PORT_PCR_IRQC_SHIFT,   //下降沿触发
    DMA_EITHER   = 0x03 << PORT_PCR_IRQC_SHIFT,   //跳变沿触发


    HDS          = 0x01 << PORT_PCR_DSE_SHIFT,    //输出高驱动能力
    PF           = 0x01 << PORT_PCR_PFE_SHIFT,    //带无源滤波器
    SSR          = 0x01 << PORT_PCR_SRE_SHIFT,    //输出慢变化率          Slow slew rate

    //下拉上拉选择
    PULLDOWN     = 0x02 << PORT_PCR_PS_SHIFT,     //下拉
    PULLUP       = 0x03 << PORT_PCR_PS_SHIFT,     //上拉

    //功能复用选择(如果不需要改变功能复用选择，保留原先的功能复用，直接选择ALT0 )
    //需要查 K60 Signal Multiplexing and Pin Assignments
    ALT0         = 0x00 << PORT_PCR_MUX_SHIFT,
    ALT1         = 0x01 << PORT_PCR_MUX_SHIFT,    //GPIO
    ALT2         = 0x02 << PORT_PCR_MUX_SHIFT,
    ALT3         = 0x03 << PORT_PCR_MUX_SHIFT,
    ALT4         = 0x04 << PORT_PCR_MUX_SHIFT,
    ALT5         = 0x05 << PORT_PCR_MUX_SHIFT,
    ALT6         = 0x06 << PORT_PCR_MUX_SHIFT,
    ALT7         = 0x07 << PORT_PCR_MUX_SHIFT,
} port_cfg;

extern volatile struct  PORT_MemMap *PORTX[5];


void  port_init         (PTxn ptxn, uint32_t cfg);   //PORT初始化(配置 MUX 复用功能)
void  port_init_NoALT   (PTxn ptxn, uint32_t cfg);   //PORT初始化(不改变 MUX 复用功能)


//如下函数可用在 PORT 中断复位函数里，func就是对应管脚的中断复位函数
//具体用法可参考 porta_handler
#define PORT_FUNC(X, PTxn, func)       do{                                              \
                                        if(PORT##X##_ISFR & (1 << PTn(PTxn)))           \
                                        {                                               \
                                            PORT##X##_ISFR  = (1 << PTn(PTxn));         \
                                            func();                                     \
                                        }                                               \
                                    }while(0)


#endif      //__PORT_H__
