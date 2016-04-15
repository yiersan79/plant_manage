//==========================================================================
//文件名称：RTC.c
//功能概要：KL25 RTC底层驱动程序源文件
//版权所有：苏州大学飞思卡尔嵌入式中心(sumcu.suda.edu.cn)
//版本更新：2013-3-23  V2.0  柏祥
//==========================================================================
#include "include.h"
#include "rtc.h"

//===============================================================
//函数名称：hw_rtc_init
//函数参数：SecendTimes:定时器秒寄存器的初始值
//函数返回：无
//功能概要：RTC驱动初始化
//===============================================================
void rtc_init(uint_32 seconds, uint_32 alarm)
{
	  //时能RTC模块时钟
	  SIM_SCGC6 |= SIM_SCGC6_RTC_MASK;
	  //软件复位
	  disable_irq(RTC_alarm_irq_no);
	  disable_irq(RTC_second_irq_no);

	  RTC_CR  = RTC_CR_SWR_MASK;
	  RTC_CR  &= ~RTC_CR_SWR_MASK;

	  if (RTC_SR & RTC_SR_TIF_MASK)
	  {
	      RTC_TSR = 0x00000000;
	  }
	  RTC_TCR = RTC_TCR_CIR(0) | RTC_TCR_TCR(0);
	  //使能计数器
	  if (seconds >0)
	  {
	    enable_irq(RTC_second_irq_no);
	    //选择呢LPO作为RTC时钟源
	    SIM_SOPT1 |= SIM_SOPT1_OSC32KSEL(3);
	    RTC_IER |= RTC_IER_TSIE_MASK;
	    RTC_SR |= RTC_SR_TCE_MASK;
	    RTC_TSR = seconds;
	  }
	  else
	  {
	    RTC_IER &= ~RTC_IER_TSIE_MASK;
	  }
	  if (alarm >0)
	  {
	    RTC_IER |= RTC_IER_TAIE_MASK;
	    RTC_SR |= RTC_SR_TCE_MASK;
	    RTC_TAR = alarm;
	    enable_irq(RTC_alarm_irq_no);
	    //选择呢LPO作为RTC时钟源
	    SIM_SOPT1 |= SIM_SOPT1_OSC32KSEL(3);  //选择LPO 1khz作为时钟输入
	  }
	  else
	  {
	    RTC_IER &= ~RTC_IER_TAIE_MASK;
	  }
}
