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
#include "common.h"   //包含公共要素头文件
#include "uart.h"
#include "freedom.h"

//串口1、2号地址映射
const UART_MemMapPtr UART_ARR[] = {UART1_BASE_PTR, UART2_BASE_PTR};

/*************************************************************************
*                             蓝宙电子工作室
*
*  函数名称：uart_init
*  功能说明：初始化uart模块
*  参数说明：UARTn       模块号（UART0~UART2）
*  函数返回：无
*  修改时间：2012-1-20
*  备    注：官方例程，printf会调用这函数
*************************************************************************/
void uart_init (UARTn uratn,uint32_t baud_rate)
{
  //局部变量声明
  register uint_16 sbr;
  uint8_t temp;
  uint16_t  clk_kh ;
  UARTLP_MemMapPtr uartch1=UART0_BASE_PTR;//声明uartch1为UARTLP_MemMapPtr结构体类型指针
  UART_MemMapPtr uartch2;     //声明uartch2为UART_MemMapPtr结构体类型指针
  uartch2 = UART_ARR[uratn-1]; //获得UART1、2模块相应口基地址
  
  //根据带入参数uartNo，给局部变量uartch1赋值
  
  if(uratn==0)
  {
    
    
    SIM_SCGC4 |= SIM_SCGC4_UART0_MASK;      //使能 UART0 时钟
    
    if(UART0_RX == PTA1)
    {
      port_init( UART0_RX, ALT2);
    }
    else if((UART0_RX == PTA15)  || (UART0_RX == PTB16) || (UART0_RX == PTD6) )
    {
      port_init( UART0_RX, ALT3);
    }
    else
    {
      ASSERT(0);                           //上诉条件都不满足，直接断言失败了，设置管脚有误？
    }
    
    if(UART0_TX == PTA2)
    {
      port_init( UART0_TX, ALT2);
    }
    else if((UART0_TX == PTA14)  || (UART0_TX == PTB17) || (UART0_TX == PTD7) )
    {
      port_init( UART0_TX, ALT3);
    }
    else
    {
      ASSERT(0);                           //上诉条件都不满足，直接断言失败了，设置管脚有误？
    }
    
    SIM_SOPT2 |= SIM_SOPT2_UART0SRC(1);   //UART0选择MCGFLLCLK_khz=48000Khz时钟源
    SIM_SOPT2 |= SIM_SOPT2_PLLFLLSEL_MASK;
    
    //暂时关闭串口0发送与接收功能
    uartch1->C2 &= ~(UART_C2_TE_MASK | UART_C2_RE_MASK);
    
    //配置串口工作模式:8位无校验模式
    clk_kh = core_clk_khz/2 ;
    sbr = (uint_16)((clk_kh*1000)/(baud_rate * 16));
    temp = UART_BDH_REG(uartch1) & ~(UART_BDH_SBR(0x1F));
    UART_BDH_REG(uartch1) = temp |  UART_BDH_SBR(((sbr & 0x1F00) >> 8));
    UART_BDL_REG(uartch1) = (uint8_t)(sbr & UART_BDL_SBR_MASK);
    
    //初始化控制寄存器、清标志位
    UART0_C4 = 0x0F;
    UART0_C1 = 0x00;
    UART0_C3 = 0x00;
    UART0_MA1 = 0x00;
    UART0_MA2 = 0x00;
    UART0_S1 |= 0x1F;
    UART0_S2 |= 0xC0;
    
    //启动发送接收
    uartch1->C2 |= (UART_C2_TE_MASK | UART_C2_RE_MASK);
  }
  else
  {
    switch (uratn)
    {
    case UART1:
      SIM_SCGC4 |= SIM_SCGC4_UART1_MASK;
      
      if((UART1_RX == PTA18) || (UART1_RX == PTC3) || (UART1_RX == PTE1) )
      {
        port_init( UART1_RX, ALT3);
      }
      else
      {
        ASSERT(0);                           //上诉条件都不满足，直接断言失败了，设置管脚有误？
      }		         
      if((UART1_TX == PTA19)  || (UART1_TX == PTC4) || (UART1_TX == PTE0) )
      {
        port_init( UART1_TX, ALT3);
      }
      else
      {
        ASSERT(0);                           //上诉条件都不满足，直接断言失败了，设置管脚有误？
      }
      
      break ;
      
    case UART2:
      
      SIM_SCGC4 |= SIM_SCGC4_UART2_MASK;
      
      if((UART2_RX == PTD2) || (UART2_RX == PTD4) || (UART2_RX == PTE17))
      {
        port_init( UART2_RX, ALT3);
      }
      else if(UART2_RX == PTE23)
      {
        port_init( UART2_RX, ALT4);
      }
      else
      {
        ASSERT(0);                           //上诉条件都不满足，直接断言失败了，设置管脚有误？
      }
      
      if((UART2_TX == PTD3) || (UART2_TX == PTD5) || (UART2_TX == PTE16))
      {
        port_init( UART2_TX, ALT3);
      }
      else if(UART2_TX == PTE22)
      {
        port_init( UART2_TX, ALT4);
      }
      break;
    default:  break;
    }
    
    //暂时关闭串口1、2发送与接收功能
    uartch2->C2 &= ~(UART_C2_TE_MASK | UART_C2_RE_MASK);
    
    //配置波特率,KL25串口1、2时钟频率只能使用 Bus clock = 24M 总线时钟
    //配置串口工作模式,8位无校验模式
    uartch2->C1 = 0;
    sbr = (uint_16)((bus_clk_khz*1000)/(baud_rate * 16));
    temp = UART_BDH_REG(uartch2) & ~(UART_BDH_SBR(0x1F));
    UART_BDH_REG(uartch2) = temp |  UART_BDH_SBR(((sbr & 0x1F00) >> 8));
    UART_BDL_REG(uartch2) = (uint8_t)(sbr & UART_BDL_SBR_MASK);
    
    //初始化控制寄存器、清标志位
    uartch2->C1 = 0x00;
    uartch2->C3 = 0x00;
    uartch2->S1 = 0x1F;
    uartch2->S2 = 0x00;
    //启动发送接收
    uartch2->C2 |= (UART_C2_TE_MASK | UART_C2_RE_MASK);
  }
}

/*************************************************************************
*                             蓝宙电子工作室
*
*  函数名称：uart_getchar
*  功能说明：无限时间等待串口接受一个字节
*  参数说明：UARTn       模块号（UART0~UART5）
*  函数返回：接收到的字节
*  修改时间：2012-1-20
*  备    注：官方例程
*************************************************************************/
char uart_getchar (UARTn uratn)
{
  UARTLP_MemMapPtr uartch1=UART0_BASE_PTR;
  UART_MemMapPtr uartch2 = UART_ARR[uratn-1];
  if(0==uratn)
  {
    /* Wait until character has been received */
    while (!((uartch1->S1) & UARTLP_S1_RDRF_MASK ));
    
    /* Return the 8-bit data from the receiver */
    return uartch1->D;
  }
  else
  {
    /* Wait until character has been received */
    while (!((uartch2->S1) & UART_S1_RDRF_MASK));
    
    /* Return the 8-bit data from the receiver */
    return uartch2->D;
    
  }
  
}
/*************************************************************************
*                             蓝宙电子工作室
*
*  函数名称：uart_pendchar
*  功能说明：有限时间等待串口接受一个字节
*  参数说明：UARTn       模块号（UART0~UART5）
*  函数返回：接收到的字节
*  修改时间：2012-1-20
*  备    注：
*************************************************************************/
uint8_t uart_pendchar (UARTn uratn, uint8_t *fp)
{
  uint32_t t;
  uint8_t  dat;
  UARTLP_MemMapPtr uartch1=UART0_BASE_PTR;
  UART_MemMapPtr uartch2 = UART_ARR[uratn-1];
  
  for (t = 0; t < 0xFBBB; t++)//查询指定次数
  {
    if(0==uratn)
    {
      //判断接收缓冲区是否满
      if ((uartch1->S1) & UART_S1_RDRF_MASK )
      {
        dat = uartch1->D;
        *fp= 0;  //收到数据
        break;
      }
    }
    else
    {
      //判断接收缓冲区是否满
      if ((uartch2->S1) & UART_S1_RDRF_MASK )
      {
        dat = uartch2->D;
        *fp= 0;  //收到数据
        break;
      }
    }
    
  }//end for
  if(t >= 0xFBBB)
  {
    dat = 0xFF;
    *fp = 1;  //未收到数据
  }
  return dat;    //返回接收到的数据
  
}

/*************************************************************************
*                             蓝宙电子工作室
*
*  函数名称：uart_pendstr
*  功能说明：有限时间等待串口接受字符串
*  参数说明：UARTn       模块号（UART0~UART5）
*  函数返回：接收到的字节
*  修改时间：2012-1-20
*  备    注：
*************************************************************************/
uint8_t uart_pendstr (UARTn uratn, uint8_t *str)
{
    uint32_t i = 0;
    while(uart_pendchar(uratn, str + i++));

    return (i <= 1 ? 0 : 1);
}

/*************************************************************************
*                             蓝宙电子工作室
*
*  函数名称：uart_query
*  功能说明：查询是否接受到一个字节
*  参数说明：UARTn       模块号（UART0~UART5）
*  函数返回：1           接收到一个字节了
*            0           没有接收到
*  修改时间：2012-1-20
*  备    注：官方例程
*************************************************************************/
int uart_query (UARTn uratn)
{
  UARTLP_MemMapPtr uartch1=UART0_BASE_PTR;
  UART_MemMapPtr uartch2 = UART_ARR[uratn-1];
  
  if(0==uratn)
  {
    return ((uartch1->S1) & UARTLP_S1_RDRF_MASK );
  }
  else
  {
   return ((uartch2->S1) & UARTLP_S1_RDRF_MASK);
  }  
    
}

/*************************************************************************
*                             蓝宙电子工作室
*
*  函数名称：uart_putchar
*  功能说明：串口发送一个字节
*  参数说明：UARTn       模块号（UART0~UART5）
*  函数返回：无
*  修改时间：2012-1-20
*  备    注：官方例程，printf会调用这函数
*************************************************************************/
void uart_putchar (UARTn uratn, char ch)
{
  UARTLP_MemMapPtr uartch1=UART0_BASE_PTR;
  UART_MemMapPtr uartch2 = UART_ARR[uratn-1];
  
  if(0==uratn)
  {
    //等待发送缓冲区空
    while(!((uartch1->S1) & UARTLP_S1_TDRE_MASK));
    //发送数据
    uartch1->D = ch;
  }
  else
  {
    //等待发送缓冲区空
    while(!((uartch2->S1) & UART_S1_TDRE_MASK));
    //发送数据
    uartch2->D = ch;
    
  }
   
}


/*************************************************************************
*                             蓝宙电子工作室
*
*  函数名称：uart_sendN
*  功能说明：串行 发送指定len个字节长度字符串 （包括 NULL 也会发送）
*  参数说明：UARTn       模块号（UART0~UART5）
*            buff        发送缓冲区
*            len         发送长度
*  函数返回：无
*  修改时间：2012-1-20
*  备    注：
*************************************************************************/
void uart_sendN (UARTn uratn, uint8_t *buff, uint16_t len)
{
    int i;
    for(i = 0; i < len; i++)
    {
        uart_putchar(uratn, buff[i]);
    }
}

/*************************************************************************
*                             蓝宙电子工作室
*
*  函数名称：uart_sendStr
*  功能说明：串行发送字符串
*  参数说明：UARTn       模块号（UART0~UART5）
*            str         字符串
*  函数返回：无
*  修改时间：2012-1-20
*  备    注：
*************************************************************************/
void uart_sendStr (UARTn uratn, const uint8_t *str)
{
    while(*str)
    {
        uart_putchar(uratn, *str++);
    }
}

/*************************************************************************
*                             蓝宙电子工作室
*
*  函数名称：uart_irq_EN
*  功能说明：开串口接收中断
*  参数说明：UARTn       模块号（UART0~UART2）
*  函数返回：无
*  修改时间：2014-9-17
*  备    注：
*************************************************************************/
void uart_irq_EN(UARTn uratn)
{
  UARTLP_MemMapPtr uartch1 = UART0_BASE_PTR;
  UART_MemMapPtr uartch2 = UART_ARR[uratn-1];
  if(0==uratn)
  {
    uartch1->C2 |= UART_C2_RIE_MASK;        //开放UART接收中断
  }
  else
    uartch2->C2 |= UART_C2_RIE_MASK;        //开放UART接收中断
  enable_irq( uratn+12 );   //开中断控制器IRQ中断
}

/*************************************************************************
*                             蓝宙电子工作室
*
*  函数名称：uart_irq_DIS
*  功能说明：关串口接收中断
*  参数说明：UARTn       模块号（UART0~UART2）
*  函数返回：无
*  修改时间：2014-9-17
*  备    注：
*************************************************************************/
void uart_irq_DIS(UARTn uratn)
{
  UARTLP_MemMapPtr uartch1=UART0_BASE_PTR;
  UART_MemMapPtr uartch2 = UART_ARR[uratn-1];
  if(0==uratn)
    uartch1->C2 &= ~UART_C2_RIE_MASK;               //禁止UART接收中断
  else
    uartch2->C2 &= ~UART_C2_RIE_MASK;               //禁止UART接收中断
  disable_irq( uratn+12 );          //禁止中断控制器IRQ中断
}










