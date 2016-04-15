//============================================================================
//文件名称：hw_tsi.c
//功能概要：K60 tsi底层驱动程序文件
//版权所有：苏州大学飞思卡尔嵌入式中心(sumcu.suda.edu.cn)
//版本更新：2012-11-25  V1.0   初始版本
//============================================================================

#include "tsi.h"
#include "common.h"
//============================================================================
//函数名称：hw_tsi_init                                                  
//功能概要：初始化TSI模块                                                  
//参数说明：chnlIDs:8位无符号数，表示哪个通道可以使能                                                                                  
//函数返回： 无                                                               
//============================================================================

void hw_tsi_init(uint_8 chnlID)
{
	//开启TSI时钟
	SIM_SCGC5 |= (SIM_SCGC5_TSI_MASK) | (SIM_SCGC5_PORTA_MASK);
	//通道5使能
	PORTA_PCR4 = PORT_PCR_MUX(0); 
    //配置TSI通用寄存器
	 TSI0_GENCS |= (TSI_GENCS_REFCHRG(4)
				| TSI_GENCS_DVOLT(0)
				| TSI_GENCS_EXTCHRG(6)
				| TSI_GENCS_PS(2)
				| TSI_GENCS_NSCN(11)
				| TSI_GENCS_TSIIEN_MASK
				| TSI_GENCS_STPE_MASK
	);
	 
    TSI0_GENCS &= ~TSI_GENCS_ESOR_MASK;
    TSI0_GENCS &= ~TSI_GENCS_STM_MASK;
	//清越值标志位和扫描完成位
	TSI0_GENCS |= (TSI_GENCS_OUTRGF_MASK) | (TSI_GENCS_EOSF_MASK);
	//选择通道
	TSI0_DATA |= (TSI_DATA_TSICH(chnlID)); 
	//TSI使能
	TSI0_GENCS |= (TSI_GENCS_TSIEN_MASK);
}


//============================================================================
//函数名称：hw_tsi_get_value16                                                  
//功能概要：获取TSI通道的计数值                                                  
//参数说明：values:16位无符号数，保存通道计数值，                                                                                    
//函数返回： 获取TSI通道的计数值                                                                
//============================================================================
uint_16 hw_tsi_get_value16()
{

	uint_16 value;        
    TSI0_GENCS &= ~TSI_GENCS_TSIIEN_MASK;    
	//开启软件中断
	TSI0_DATA |= TSI_DATA_SWTS_MASK;
    //等待扫描完成   
	while(!(TSI0_GENCS & TSI_GENCS_EOSF_MASK));
	TSI0_GENCS |= TSI_GENCS_EOSF_MASK;
    //读取计数寄存器中的值
	value =  (TSI0_DATA & TSI_DATA_TSICNT_MASK); 
	//清超出范围标志位
    TSI0_GENCS |= TSI_GENCS_OUTRGF_MASK; 
    //清扫描结束标志位    
    TSI0_GENCS |= TSI_GENCS_EOSF_MASK;
    //TSI中断使能
    TSI0_GENCS |= TSI_GENCS_TSIIEN_MASK;      
    return value; 
}


//============================================================================
//函数名称：hw_tsi_set_threshold1                                                  
//功能概要：设定指定通道的阈值                                                  
//参数说明：     low:   设定阈值下限                                              
//           high:   设定阈值上限                                                                                      
//函数返回： 无                                                             
//============================================================================
void hw_tsi_set_threshold(uint_16 low, uint_16 high)
{
    uint_32 thresholdValue;
    //高16位为上限，低16位为下限
    thresholdValue = high;
    thresholdValue = (thresholdValue<<16)|low;
    TSI0_TSHD = thresholdValue;
}


//=========================================================================
//函数名称：tsi_enable
//参数说明：无
//函数返回：无
//功能概要：开串口接收中断
//=========================================================================
void tsi_enable()
{
    TSI0_GENCS |= (TSI_GENCS_TSIIEN_MASK|TSI_GENCS_STM_MASK);
    //开中断控制器IRQ中断
    enable_irq(26);   
}

//=========================================================================
//函数名称：tsi_disable
//参数说明：无
//函数返回：无
//功能概要：关串口接收中断
//=========================================================================
void tsi_disable()
{
	TSI0_GENCS &= ~(TSI_GENCS_TSIIEN_MASK|TSI_GENCS_STM_MASK);
	//禁止中断控制器IRQ中断
	disable_irq(26);          
}


//=========================================================================
//函数名称：tsi_softsearch
//参数说明：无
//函数返回：无
//功能概要：启动软件查询中断测试
//=========================================================================
void tsi_softsearch()
{
    //开启软件触发
	TSI0_GENCS &= ~TSI_GENCS_STM_MASK;
	//开始一次软件扫描
	TSI0_DATA |= TSI_DATA_SWTS_MASK;
}

