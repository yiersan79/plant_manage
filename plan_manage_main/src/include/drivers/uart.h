//============================================================================
//文件名称：uart.h
//功能概要：UART底层驱动构件头文件
//版权所有：苏州大学飞思卡尔嵌入式中心(sumcu.suda.edu.cn)
//更新记录：2012-11-12   V1.0
//============================================================================

#ifndef UART_H         //防止重复定义（ 开头)
#define UART_H

#include "common.h"   //包含公共要素头文件
#include "sysinit.h"

typedef enum  UARTn
{
    //初始化默认配置       --TXD--      --RXD--     可以复用其他通道，请自行修改 uart_init
    UART0,    //           PTD7         PTD6
    UART1,    //           PTC4         PTC3
    UART2,    //           PTD3         PTD2

} UARTn;

void uart_init (UARTn uratn,uint32_t baud_rate);

char uart_getchar (UARTn uratn);                     //无限等待接受1个字节
uint8_t uart_pendchar (UARTn uratn,uint8_t *fp);    //有限时间等待接收一个字符
uint8_t uart_pendstr (UARTn uratn, uint8_t *str);   //有限时间等待接收字符串

int uart_query (UARTn uratn);                                 //查询是否接收到一个字节
void uart_putchar (UARTn uratn, char ch);                    //发送1个字节
void uart_sendN (UARTn uratn, uint8_t *buff, uint16_t len);  //发送n个字节
void uart_sendStr (UARTn uratn, const uint8_t *str);         //发送字符串

void uart_irq_EN(UARTn uratn);                               //开串口接收中断
void uart_irq_DIS(UARTn uratn);                               //关串口接收中断 


#endif     //防止重复定义（ 结尾)
