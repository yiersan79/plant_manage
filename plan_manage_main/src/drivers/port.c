/******************** (C) COPYRIGHT 2011 蓝宙电子工作室 ********************
* 文件名       ： port.c
* 描述         ：工程模版实验
*
* 实验平台     ：landzo电子开发版
* 库版本       ：
* 嵌入系统     ：
*
* 作者         ：landzo 蓝电子
* 淘宝店       ：http://landzo.taobao.com/

**********************************************************************************/
#include "port.h"


/*************************************************************************
*                             蓝宙电子工作室
*
*  函数名称：port_init
*  功能说明：PORT初始化
*  参数说明：ptxn   端口
*            cfg     端口属性配置，如触发选项和上拉下拉选项
*  函数返回：无
* 作者      ：野火工作室\landzo 蓝电子
*  修改时间：2014-9-18   已测试
*  备    注：与port_init_NoALT不同的是，此函数需要配置 MUX 复用功能，
*                否则 MUX = ALT0
*  Sample usage:       port_init (PTA8, IRQ_RISING | PF | ALT1 | PULLUP );    
*                     //初始化 PTA8 管脚，上升沿触发中断，带无源滤波器，复用功能为GPIO ，上拉电阻
*************************************************************************/
void  port_init(PTxn ptxn, uint32_t cfg )
{
    SIM_SCGC5 |= (SIM_SCGC5_PORTA_MASK << PTX(ptxn));       //开启PORTx端口

    PORT_PCR_REG(PORTX_BASE(ptxn), PTn(ptxn)) = cfg;         // 复用功能 , 确定触发模式 ,开启上拉或下拉电阻
}

/*************************************************************************
*                             蓝宙电子工作室
*
*  函数名称：port_init_NoALT
*  功能说明：PORT初始化
*  参数说明：ptxn   端口
*            cfg     端口属性配置，如触发选项和上拉下拉选项
*  函数返回：无
* 作者      ：野火工作室\landzo 蓝电子
*  修改时间：2014-9-18   
*  备    注：与port_init不同的是，此函数不需要配置 MUX 复用功能（即使配置了也不生效），
*                MUX 保留 为原先寄存器配置的值
*  Sample usage:       port_init_NoALT (PTA8, IRQ_RISING | PF | PULLUP );    
*                //初始化 PTA8 管脚，上升沿触发中断，带无源滤波器，保留原先复用功能，上拉电阻 
*************************************************************************/
void  port_init_NoALT(PTxn ptxn, uint32_t cfg)
{
    SIM_SCGC5 |= (SIM_SCGC5_PORTA_MASK << PTX(ptxn));     //开启PORTx端口

    //清空cfg里的MUX，加载寄存器里的MUX
    cfg &= ~PORT_PCR_MUX_MASK;                      //清了MUX 字段（即不需要配置ALT，保持原来的ALT）
    cfg |=  (PORT_PCR_REG(PORTX_BASE(ptxn), PTn(ptxn)) & PORT_PCR_MUX_MASK);    //读取寄存器里配置的 MUX

    PORT_PCR_REG(PORTX_BASE(ptxn), PTn(ptxn)) = cfg;            // 复用功能 , 确定触发模式 ,开启上拉或下拉电阻
}

