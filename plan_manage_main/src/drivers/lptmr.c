/******************** (C) COPYRIGHT 2011 蓝宙电子工作室 ********************
* 文件名       ： lptmr.c
* 描述         ：工程模版实验
*
* 实验平台     ：landzo电子开发版
* 库版本       ：
* 嵌入系统     ：
*
* 作者         ：landzo 蓝电子
* 淘宝店       ：http://landzo.taobao.com/

**********************************************************************************/
#include "lptmr.h"


/*************************************************************************
*                             蓝宙电子工作室
*
*  函数名称：lptmr_internal_ref_init
*  功能说明：内部参考时钟初始化 采用快速内部参考时钟，1s中断一次
*  参数说明：无
*  函数返回：无
*  修改时间：2014-9-18
*  备    注：
*************************************************************************/
void lptmr_internal_ref_init()
{
	uint32_t compare_value=1953;
	MCG_C1|=MCG_C1_IRCLKEN_MASK;      //使能内部参考时钟
	MCG_C2|=MCG_C2_IRCS_MASK;		  //MCG_C[IRCS]=1,使能快速内部参考时钟（4MHz）
	MCG_SC |= MCG_SC_FCRDIV(0x00) ;
	MCG_C1|=MCG_C1_IREFSTEN_MASK;     //停止模式的时候内部时钟保持使能
	SIM_SCGC5|=SIM_SCGC5_LPTMR_MASK;  //使能LPT模块时钟

	LPTMR0_PSR=LPTMR_PSR_PCS(0x00)|LPTMR_PSR_PRESCALE(0x09);//使用内部时钟，1024预分频
	LPTMR0_CMR=LPTMR_CMR_COMPARE(compare_value);            //设置比较寄存器值
	LPTMR0_CSR&=(~LPTMR_CSR_TEN_MASK);
	LPTMR0_CSR&=(~LPTMR_CSR_TFC_MASK);
	LPTMR0_CSR&=(~LPTMR_CSR_TMS_MASK);
	LPTMR0_CSR|=LPTMR_CSR_TEN_MASK;   //开启LPT模块设置
	LPTMR0_CSR|=LPTMR_CSR_TIE_MASK; //开启LPT定时器中断
}

/*************************************************************************
*                             蓝宙电子工作室
*
*  函数名称：lptmr_count_init
*  功能说明：LPTMR脉冲计数初始化
*  参数说明：LPT0_ALTn   LPTMR脉冲计数管脚
*            count       LPTMR脉冲比较值
*            LPT_CFG     LPTMR脉冲计数方式：上升沿计数或下降沿计数
*  函数返回：无
*  修改时间：2014-9-18
*  备    注： 
*  Sample usage:       lptmr_pulse_init(LPT0_ALT1,0xFFFF,LPT_Rising);     
* // LPTMR 脉冲捕捉，捕捉0xFFFF后触发中断请求（需要开中断才执行中断复位函数），上升沿捕捉 
*************************************************************************/
void lptmr_counter_init(LPT0_ALTn altn, uint16_t count, LPT_CFG cfg)
{

    SIM_SCGC5 |= SIM_SCGC5_LPTMR_MASK;                        //使能LPT模块时钟

    //设置输入管脚
    if(altn == LPT0_ALT1)
    {
        port_init(PTA19, ALT6 );            //在PTA19上使用 ALT6
    }
    else if(altn == LPT0_ALT2)
    {
        port_init(PTC5, ALT3 );             //在PTC5上使用 ALT3
    }
    else                                    //不可能发生事件
    {
        ASSERT((altn == LPT0_ALT1 ) || (altn == LPT0_ALT2 ));   //设置管脚有误？
    }

    // 清状态寄存器
    LPTMR0_CSR = 0x00 ;                                          //先关了LPT，这样才能设置时钟分频,清空计数值等
    LPTMR0_PSR = 0x00 ;
    LPTMR0_CSR = 0x00 ;
    LPTMR0_CMR = 0X00 ; 
   
    // 设置累加计数值
    LPTMR0_CMR  =   LPTMR_CMR_COMPARE(count);                   //设置比较值

    LPTMR0_PSR  =   (0
                     |LPTMR_PSR_PCS(0x00)
                     |LPTMR_PSR_PBYP_MASK
                     |LPTMR_PSR_PRESCALE(0x01)); 
    // 管脚设置、使能中断
    
    LPTMR0_CSR  =  (0
                    | LPTMR_CSR_TPS(altn)       // 选择输入管脚 选择
                    | LPTMR_CSR_TMS_MASK        // 选择脉冲计数 (注释了表示时间计数模式)
                    | ( cfg == LPT_Falling ?  LPTMR_CSR_TPP_MASK :   0  )  //脉冲计数器触发方式选择：0为高电平有效，上升沿加1
                    //| LPTMR_CSR_TEN_MASK        //使能LPT(注释了表示禁用)
                    //| LPTMR_CSR_TIE_MASK        //中断使能
                    | LPTMR_CSR_TFC_MASK      //0:计数值等于比较值就复位；1：溢出复位（注释表示0）
                   );

    LPTMR0_CSR  |= LPTMR_CSR_TEN_MASK ; 
   // LPTMR0_CNR = 0X03 ; 
}

/*************************************************************************
*                             蓝宙电子工作室
*
*  函数名称：lptmr_pulse_get
*  功能说明：获取LPTMR脉冲计数值
*  参数说明：无
*  函数返回：无
*  修改时间：2014-9-18
*  备    注：  读取LPTMR0_CNR数据，必须先写入，才可读读取
*  Sample usage:       uint16_t data = lptmr_pulse_get();  //获取脉冲计数值
*************************************************************************/
uint16_t lptmr_pulse_get(void)
{
    uint16_t  LPTCOUNT = 0  ;
    LPTMR0_CNR = 12 ;
    LPTCOUNT = LPTMR0_CNR ;
    lptmr_pulse_clean();
    return (uint16)LPTCOUNT;
}


/*************************************************************************
*                             蓝宙电子工作室
*
*  函数名称：lptmr_pulse_clean
*  功能说明：清空LPTMR脉冲计数
*  参数说明：无
*  函数返回：无
*  修改时间：2014-9-18
*   作者   ：野火工作室/landzo 蓝电子
*  备    注：  
*************************************************************************/
void lptmr_pulse_clean(void)
{
    LPTMR0_CSR  &= ~LPTMR_CSR_TEN_MASK;     //禁用LPT的时候就会自动清计数器的值
    LPTMR0_CSR  |= LPTMR_CSR_TEN_MASK;
}


//============================================================================
//函数名称：enable_lptmr_int
//函数返回：无
//参数说明：无
//功能概要：开启LPT定时器中断
//============================================================================
void enable_lptmr_int()
{
	//LPTMR0_CSR|=LPTMR_CSR_TIE_MASK; //开启LPT定时器中断
	enable_irq(LPTMRLPTMR_irq);	    //开引脚的IRQ中断
}

//============================================================================
//函数名称：disable_lptmr_int
//函数返回：无
//参数说明：无
//功能概要：关闭LPT定时器中断
//============================================================================
void disable_lptmr_int()
{
	//LPTMR0_CSR &=~LPTMR_CSR_TIE_MASK;   //禁止LPT定时器中断
	disable_irq(LPTMRLPTMR_irq);	    //关引脚的IRQ中断
}


