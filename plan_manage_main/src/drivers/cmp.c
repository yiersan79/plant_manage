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
#include "cmp.h"

//============================================================================
//函数名称：hw_cmp_init
//函数返回     无
//参数说明    reference:参考电压选择  0=VDDA 3.3V 1=VREF 1.2V
//         plusChannel: 正比较通道号
//         minusChannel：负比较通道号 
//         通道0,1,2,3 对应 PTC6， PTC7， PTC8， PTC9
//功能概要：CMP模块初始化
//============================================================================
void cmp_init(uint8_t reference,uint8_t plusChannel,uint8_t minusChannel)
{
      //通过获取模块号选择比较器基址
    CMP_MemMapPtr cmpch = CMP0_BASE_PTR;
    
    if(plusChannel>7)
      plusChannel = 7;
    if(plusChannel<=0)
      plusChannel = 0;
    
    if(minusChannel>7)
      minusChannel = 7;
    if(minusChannel<=0)
      minusChannel = 0;
    
    //使能比较模块时钟
    SIM_SCGC4 |=SIM_SCGC4_CMP_MASK;
        
    //初始化寄存器
    CMP_CR0_REG(cmpch) = 0;
    CMP_CR1_REG(cmpch) = 0;
    CMP_FPR_REG(cmpch) = 0;
    //如果设置了标志清除中断标志
    CMP_SCR_REG(cmpch) = 0x06;  
    CMP_DACCR_REG(cmpch) = 0;
    CMP_MUXCR_REG(cmpch) = 0;
    
    //配置寄存器
    //过滤，数字延时禁止
    CMP_CR0_REG(cmpch) = 0x00;  
    //连续模式，高速比较，无过滤输出，输出引脚禁止
    CMP_CR1_REG(cmpch) = 0x16;  
    //过滤禁止
    CMP_FPR_REG(cmpch) = 0x00; 
    //使能上升沿和下降沿中断，清标志位
    CMP_SCR_REG(cmpch) = 0x1E;  
    
    
    if(reference==0)//参考电压选择VDD3.3V
    {   
      //6位参考DAC使能，选择VDD作为DAC参考电压
      CMP_DACCR_REG(cmpch) |= 0xC0;
    }
    else//参考电压选择VREF OUT 1.2V
    {
      //6位参考DAC使能，选择VREF作为DAC参考电
      CMP_DACCR_REG(cmpch) |= 0x80;
    }
    
    
    CMP_MUXCR_REG(cmpch) = CMP_MUXCR_PSEL(plusChannel)//正通道选择
                     | CMP_MUXCR_MSEL(minusChannel);//负通道选择    
    
    
    //选择输出引脚
    PORTC_PCR5=PORT_PCR_MUX(6);
        //CMP使能   
    CMP_CR1_REG(cmpch) |= CMP_CR1_EN_MASK;

}


//============================================================================
//函数名称：hw_dac_set_value
//函数返回：无
//参数说明：value: dac输出的转换值
//功能概要：开比较中。
//============================================================================
void cmp_dac_set_value(uint8_t value)
{
   //通过获取模块号选择比较器基址
   CMP_MemMapPtr cmpch = CMP0_BASE_PTR;
   CMP_DACCR_REG(cmpch) |= CMP_DACCR_VOSEL(value);
}



//============================================================================
//函数名称：hw_enable_cmp_int
//函数返回：无
//参数说明：无
//功能概要：开比较中。
//============================================================================
void enable_cmp_int()
{
  //通过获取模块号选择比较器基址
   CMP_MemMapPtr cmpch = CMP0_BASE_PTR;
  //开放cmp接收中断,上升沿下降沿均触发
   CMP_SCR_REG(cmpch)|=  CMP_SCR_IEF_MASK  | CMP_SCR_IER_MASK; 
   enable_irq(16);    
}

//============================================================================
//函数名称：hw_disable_cmp_int
//函数返回：无
//参数说明：无
//功能概要：关比较中断
//============================================================================
void disable_cmp_int()
{
  //通过获取模块号选择比较器基址
    CMP_MemMapPtr cmpch = CMP0_BASE_PTR;
  //关闭cmp接收中断,上升沿下降沿均关闭
  CMP_SCR_REG(cmpch)&=(~CMP_SCR_IEF_MASK) | (~CMP_SCR_IER_MASK );   
  //关接收引脚的IRQ中断
  disable_irq(16);
  
}


