/******************** (C) COPYRIGHT 2011 蓝宙电子工作室 ********************
* 文件名       ： gpio.c
* 描述         ：工程模版实验
*
* 实验平台     ：landzo电子开发版
* 库版本       ：
* 嵌入系统     ：
*
* 作者         ：landzo 蓝电子
* 淘宝店       ：http://landzo.taobao.com/

**********************************************************************************/
#include "gpio.h"   //包含本构件头文件

//各端口基地址放入常数数据组PORTX[0]~PORTX[4]中
volatile struct PORT_MemMap *PORTX[5]={PORTA_BASE_PTR,PORTB_BASE_PTR,
PORTC_BASE_PTR,PORTD_BASE_PTR,PORTE_BASE_PTR};
//GPIO口基地址放入常数数据组GPIOx[0]~GPIOx[4]中
volatile struct GPIO_MemMap *GPIOx[5]={PTA_BASE_PTR,PTB_BASE_PTR,
PTC_BASE_PTR,PTD_BASE_PTR,PTE_BASE_PTR};

/*************************************************************************
*                             蓝宙电子科技有限公司
*
*  函数名称：gpio_Interrupt_init
*  功能说明：初始化gpio
*  参数说明：PTxn      端口号（PORTA,PORTD）
*            IO          引脚方向,0=输入,1=输出，输入输出状态定义____________(修改：这个函数中只有定义为输入模式有效，否则不改变相关状态)
*            mode        中断模式
*  函数返回：无
*  修改时间：2012-9-15   已测试
*  备    注：
*************************************************************************/
void gpio_Interrupt_init(PTxn ptxn, GPIO_CFG cfg, GPIO_INP mode)
{
  ASSERT( (PTn(ptxn) < 32u)  );           //使用断言检查输入、电平 是否为1bit
  
  //选择功能脚 PORTx_PCRx ，每个端口都有个寄存器 PORTx_PCRx 
  
  PORT_PCR_REG(PORTX_BASE(ptxn), PTn(ptxn)) = (0 | PORT_PCR_MUX(1) | cfg | PORT_PCR_IRQC(mode) );
  //选择功能脚 PORTx_PCRx ，每个端口都有中断模型
  
//  PORT_DFER_REG(PORTX_BASE(ptxn)) = PORT_DFER_DFE( 1<<PTn(ptxn));
  
  //端口方向控制输入还是输出
  if( ( (cfg & 0x01) == GPI) || (cfg == GPI_UP) ||     (cfg == GPI_UP_PF) 
     || (cfg == GPI_DOWN) ||     (cfg == GPI_DOWN_PF)     )
    //   最低位为0则输入   ||   输入上拉模式  ||   输入上拉，带无源滤波器
  {
    GPIO_PDDR_REG(GPIOX_BASE(ptxn)) &= ~(1 << PTn(ptxn));  //设置端口方向为输入
  }
  
  if(PTX(ptxn)==0)
    enable_irq(PortA_irq_no);
  else if(PTX(ptxn)==3)
    enable_irq(PortD_irq_no);
  
}

/*************************************************************************
*                             蓝宙电子工作室
*
*  函数名称：gpio_init
*  功能说明：初始化gpio
*  参数说明：port：端口号（gpio.h中宏定义，分别为PORTA~PORTE=0~4)
*            dir：引脚方向（0=输入，1=输出）
*            state：引脚初始状态（0=低电平，1=高电平）
*  函数返回：无
*  修改时间：2014-9-18   已测试
*  备    注：
*************************************************************************/
void gpio_init(PTxn ptxn, uint8_t dir, uint8_t state)
{	
  //根据带入参数pin,指定该引脚功能为GPIO功能（即令引脚控制寄存器的MUX=0b001）
  PORT_PCR_REG(PORTX_BASE(ptxn), PTn(ptxn))= PORT_PCR_MUX(1);
  
  //根据带入参数dir，决定引脚为输出还是输入
  if (1 == dir)   //希望为输出
  {
    GPIO_PDDR_REG(GPIOX_BASE(ptxn)) |= (1 << PTn(ptxn));    //设置端口方向为输出
    gpio_set(ptxn , state);   //调用gpio_set函数，设定引脚初始状态
  }
  else         //希望为输入
    GPIO_PDDR_REG(GPIOX_BASE(ptxn)) &= ~(1 << PTn(ptxn));  //设置端口方向为输入
}

/*************************************************************************
*                             蓝宙电子工作室
*
*  函数名称：gpio_set
*  功能说明：设置引脚状态
*  参数说明：ptxn：端口号（gpio.h中宏定义,gpio_cfg.h)
*            state        输出初始状态,0=低电平,1=高电平
*  函数返回：无
*  修改时间：2012-1-16   已测试
*  备    注：_____________________________________(修改过)
*************************************************************************/
void gpio_set(PTxn ptxn, uint8_t state)
{
  if(state == 1)
  	GPIO_SET(PTX(ptxn), PTn(ptxn), 1);
  else
  	GPIO_SET(PTX(ptxn), PTn(ptxn), 0);
}


/*************************************************************************
*                             蓝宙电子工作室
*
*  函数名称：gpio_get
*  功能说明：获得引脚的状态（1或0）
*  参数说明：ptxn：端口号（gpio.h中宏定义,gpio_cfg.h)
*  函数返回：无
*  修改时间：2014-9-18  已测试
*  备    注：
*************************************************************************/
uint8_t gpio_get(PTxn ptxn)
{	
  //返回引脚的状态
  return GPIO_Get(PTX(ptxn),PTn(ptxn)) ;
}

/*************************************************************************
*                             蓝宙电子工作室
*
*  函数名称：gpio_turn
*  功能说明：指定引脚的状态（1或0）
*  参数说明：ptxn：端口号（gpio.h中宏定义,gpio_cfg.h)
*  函数返回：无
*  修改时间：2014-9-18  已测试
*  备    注：
*************************************************************************/
void gpio_turn(PTxn ptxn)
{
  GPIO_TURN(PTX(ptxn), PTn(ptxn));
}


