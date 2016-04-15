#include "SPI.h"


//==============================================================================
//函数名称：SPI_init。
//功能说明：SPI初始化。通信波特率BaudRate = BusClock /16.其中BusClock是系统时钟。
//函数参数：SPI_mode：SPI模式。
//       ：SPI_No：要初始化的SPI口号。	 
//函数返回：0:初始化失败；1：初始化成功。
//==============================================================================

uint_8 SPI_init(uint_8 SPI_No,uint_8 SPI_mode)
{
	SPI_MemMapPtr baseadd=SPI_baseadd(SPI_No);
		if(SPI_No==0)		//对把portA的14~17引脚选择为SPI0功能。
		{
			SIM_SCGC4|=SIM_SCGC4_SPI0_MASK;			//打开SPI0模块时钟。
			PORTA_PCR14=(0|PORT_PCR_MUX(0x02));		//选择PTA14的SS功能
			PORTA_PCR15=(0|PORT_PCR_MUX(0x02));		//选择PTA15的SCK功能
			PORTA_PCR16=(0|PORT_PCR_MUX(0x02));		//选择PTA16的MOSI功能
			PORTA_PCR17=(0|PORT_PCR_MUX(0x02));		//选择PTA17的MIOS功能
			
			SPI0_C1 = 0|SPI_C1_SPE_MASK
			    		|SPI_C1_MSTR_MASK
			    		|SPI_C1_SSOE_MASK;		//对SPI0的C1寄存器配置为主机模式、从机选择自动模式并
			    									//使能SPI0模块。
			SPI0_C2 = 0|SPI_C2_MODFEN_MASK;		//与SPI0中C1的SSOE配合，把从机选择配置为自动模式。
			SPI0_BR = 0x00U;					//设置波特率。
		}
		else if(SPI_No==1)		//对把portE的1~4引脚选择为SPI1功能。
		{
			SIM_SCGC4|=SIM_SCGC4_SPI1_MASK;			//打开SPI1模块时钟。
			PORTE_PCR1=(0|PORT_PCR_MUX(0x02));		//选择PTE1的MOSI功能
			PORTE_PCR2=(0|PORT_PCR_MUX(0x02));		//选择PTE2的SCK功能
			PORTE_PCR3=(0|PORT_PCR_MUX(0x02));		//选择PTE3的MISO功能
			PORTE_PCR4=(0|PORT_PCR_MUX(0x02));		//选择PTE4的MOSI功能
			
			 SPI1_C1 = 0|SPI_C1_SPE_MASK
			    		|SPI_C1_SPIE_MASK;	   //对SPI1的C1寄存器进行配置
			 enable_irq (11);				       //开启SPI1中断。
			 SPI1_C2 = 0x00U;		          
			 SPI1_BR = 0x00U;		 		      //设置与SPI0相同的波特率。
			
		}
		else return(0);
		return(1);
		
}

//==============================================================================
//函数名称：SPI_sendonebyte.
//功能说明：SPI发送一字节数据。
//函数参数：SPI_No：串口号。
//       ：ch：需要发送的一字节数据。
//函数返回：0：发送失败；1：发送成功。
//==============================================================================
uint_8 SPI_sendonebyte(uint_8 SPI_No,uint_8 ch)
{
	uint_32 i;
	SPI_MemMapPtr baseadd=SPI_baseadd(SPI_No);
	while(!SPI_S_REG(baseadd)&SPI_S_SPTEF_MASK);
	SPI_D_REG(baseadd)=ch;
	for(i=0;i<0xFFF0;i++)		//在一定时间内发送不能完成，则认为发送失败。
	{
		if(SPI_S_REG(baseadd)&SPI_S_SPTEF_MASK)
		{
			return(1);
		}
	}
	return(0);
}

//==============================================================================
//函数名称：SPI_sendstring.
//功能说明：SPI发送数据。
//函数参数：SPI_No：串口号。
//       ：*p:所发数据的首地址。
//函数返回：无。
//==============================================================================
void SPI_sendstring(uint_8 SPI_No,uint_8 *p)
{
	SPI_MemMapPtr baseadd=SPI_baseadd(SPI_No);
	uint_32 k;
	for(k=0;p[k]!='\0';k++)
	{
		while(!SPI_S_REG(baseadd)&SPI_S_SPTEF_MASK);
			SPI_D_REG(baseadd)=p[k];
	}
}

//==============================================================================
//函数名称：SPI_reonebyte.
//功能说明：SPI接收一个字节的数据
//函数参数：SPI_No：SPI口号。
//函数返回：接收到的数据。
//==============================================================================
uint_8 SPI_reonebyte(uint_8 SPI_No)
{
	SPI_MemMapPtr baseadd=SPI_baseadd(SPI_No);
	while(!(SPI_S_REG(baseadd)& SPI_S_SPRF_MASK));	//检测SPI1是否收到了数据。
	return SPI_D_REG(baseadd);				
}

//==============================================================================
//函数名称：SPI_receiveN.
//功能说明：SPI接收数据。当n=1时，就是接受一个字节的数据……
//函数参数：SPI_No：SPI口号。
//		 : n:要接收的字节个数。
//       ：data[]:接收到的数据。
// 函数返回：1：接收成功,其他情况：失败。
//==============================================================================
uint_8 SPI_receiveN(uint_8 SPI_No,uint_32 n,uint_8 data[])
{
	SPI_MemMapPtr baseadd=SPI_baseadd(SPI_No);
	uint_32 m=0;
	while(m<n)
	{
		if(SPI_S_REG(baseadd)&SPI_S_SPRF_MASK)
		{
			data[m]=SPI_D_REG(baseadd);
			m++;
		}
	}
	return(1);
}

//==============================================================================
//函数名称：SPI_re_enable_int.
//功能说明：打开SPI接收中断。
//函数参数：SPI_No：SPI口号。
//函数返回：无。
//==============================================================================
void SPI_re_enable_int(uint_8 SPI_No)
{
    enable_irq (SPI_No+10);
}

//==============================================================================
//函数名称：SPI_re_disable_int.
//功能说明：关闭SPI接收中断。
//函数参数：SPI_No：SPI口号。
//函数返回：无。
//==============================================================================
void SPI_re_disable_int(uint_8 SPI_No)
{
    disable_irq (SPI_No+10);
}
