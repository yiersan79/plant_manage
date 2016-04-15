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
#include "adc.h"

ADC_MemMapPtr ADCN[1] = {ADC0_BASE_PTR}; //定义一个指针数组保存 ADCN 的地址

/*************************************************************************
*                             蓝宙电子工作室
*
*  函数名称：adc_init
*  功能说明：AD初始化
*  参数说明：ADCn_Ch_e    ADC通道
*  函数返回：无
*  修改时间：2014-9-18
*  备    注：
*************************************************************************/
void adc_init(ADCn_Ch_e adcn_ch)
{
  
  uint8_t adcn = adcn_ch >> 5 ; 
  
  switch(adcn)
  {
  case ADC0:       /*   ADC0  */
    SIM_SCGC6 |= (SIM_SCGC6_ADC0_MASK );        //开启ADC0时钟
    SIM_SOPT7 &= ~(SIM_SOPT7_ADC0ALTTRGEN_MASK  | SIM_SOPT7_ADC0PRETRGSEL_MASK);
    SIM_SOPT7 |= SIM_SOPT7_ADC0TRGSEL(0);
    break;
    
  default:
    ASSERT(0);
  }
  
  switch(adcn_ch)
  {
    
  case ADC0_SE0:
    port_init(PTE20, ALT0);
    break;
  case ADC0_SE1:
    port_init(PTE16, ALT0);
    break;
  case ADC0_SE2:
    port_init(PTE18, ALT0);
    break;
  case ADC0_SE3:
    port_init(PTE22, ALT0);
    break;
  case ADC0_SE4a:
    port_init(PTE21, ALT0);
    break;
  case ADC0_SE5a:
    port_init(PTE17, ALT0);
    break;
  case ADC0_SE6a:
    port_init(PTE19, ALT0);
    break;
  case ADC0_SE7a:
    port_init(PTE23, ALT0);
    break;
  case ADC0_SE8:
    port_init(PTB0, ALT0);
    break;
  case ADC0_SE9:
    port_init(PTB1, ALT0);
    break;
  case ADC0_SE11:
    port_init(PTC2, ALT0);
    break;
  case ADC0_SE12:
    port_init(PTB2, ALT0);
    break;
  case ADC0_SE13:
    port_init(PTB3, ALT0);
    break;
  case ADC0_SE14:
    port_init(PTC0, ALT0);
    break;
  case ADC0_SE15:
    port_init(PTC1, ALT0);
    break;
  case ADC0_SE23:
    port_init(PTE30, ALT0);
    break;
    
    
  case Temp0_Sensor:   // Temperature Sensor,内部温度测量，可用ADC函数
    break;   
  case VREFH0:         // 参考高电压,可用ADC函数 ,结果恒为 2^n-1
    break;
  case VREFL0:         // 参考低电压,可用ADC函数 ,结果恒为 0
    break;
    
  default:
    ASSERT(0);      //断言，传递的管脚不支持 ADC 单端软件触发，请换 其他管脚
    break;
  }
  
  
  
}

/*************************************************************************
*                             蓝宙电子工作室
*
*  函数名称：adc_init
*  功能说明：获取ADC采样值(不支持B通道)
*  参数说明：ADCn_Ch_e    ADC通道
*            ADC_nbit     ADC精度（ ADC_8bit,ADC_12bit, ADC_10bit, ADC_16bit ）
*  函数返回：
*  修改时间：2014-9-18
*  备    注：
*************************************************************************/
uint16_t adc_once(ADCn_Ch_e adcn_ch, ADC_nbit bit) //采集某路模拟量的AD值
{
  ADCn_e adcn = (ADCn_e)(adcn_ch >> 5) ;
  
  uint16_t result = 0;
  
  adc_start(adcn_ch, bit);      //启动ADC转换
  
  while (( ADC_SC1_REG(ADCN[adcn], 0 ) & ADC_SC1_COCO_MASK ) != ADC_SC1_COCO_MASK);   //只支持 A通道
  result = ADC_R_REG(ADCN[adcn], 0);
  ADC_SC1_REG(ADCN[adcn], 0) &= ~ADC_SC1_COCO_MASK;
  return result;
}
/*************************************************************************
*                             蓝宙电子工作室
*
*  函数名称：ad_ave
*  功能说明：多次采样，取平均值
*  参数说明：ADCx        模块号（ ADC0、 ADC1）
*            ADC_Channel 通道号
*            ADC_nbit    精度（ ADC_8bit,ADC_12bit, ADC_10bit, ADC_16bit ）
*            N           均值滤波次数(范围:0~255)
*  函数返回：16位无符号结果值
*  修改时间：2012-2-10
*  备    注：修改苏州大学的例程
*************************************************************************/
uint16_t adc_ave(ADCn_Ch_e adcn_ch, ADC_nbit bit, uint8_t N) //均值滤波
{
    uint32_t tmp = 0;
    uint8_t  i;
    
    for(i = 0; i < N; i++)
        tmp += adc_once(adcn_ch, bit);
    tmp = tmp / N;
    return (uint16_t)tmp;
}

/*************************************************************************
*                             蓝宙电子工作室
*
*  函数名称：ad_SUM
*  功能说明：多次采样，取平均值
*  参数说明：ADCx        模块号（ ADC0、 ADC1）
*            ADC_Channel 通道号
*            ADC_nbit    精度（ ADC_8bit,ADC_12bit, ADC_10bit, ADC_16bit ）
*            N           均值滤波次数(范围:0~255)
*  函数返回：16位无符号结果值
*  修改时间：2014-9-18
*  备    注：
*************************************************************************/
uint16_t adc_sum(ADCn_Ch_e adcn_ch, ADC_nbit bit, u8 N) //采样集合
{
    uint32_t tmp = 0;
    uint8_t  i;    
    ADCn_e adcn = (ADCn_e)(adcn_ch >> 5) ;
    ASSERT( ((adcn == ADC0) && (ch >= AD8 && ch <= AD18)) || ((adcn == ADC1) && (ch >= AD4a && ch <= AD17)) ) ; //使用断言检测ADCn_CHn是否正常
    adc_start(adcn_ch, bit);	  //启动ADC转换
    
    for(i = 0; i < N; i++)
    {
      while (( ADC_SC1_REG(ADCN[adcn], 0 ) & ADC_SC1_COCO_MASK ) != ADC_SC1_COCO_MASK);
      tmp += ADC_R_REG(ADCN[adcn], 0);
      ADC_SC1_REG(ADCN[adcn], 0) &= ~ADC_SC1_COCO_MASK;
    }
    return (u16)tmp;
}
/*************************************************************************
*                             野火嵌入式开发工作室\蓝宙电子工作室
*
*  函数名称：adc_start
*  功能说明：启动ADC软件采样(不支持B通道)
*  参数说明：ADCn_Ch_e    ADC通道
*            ADC_nbit    精度（ ADC_8bit,ADC_12bit, ADC_10bit, ADC_16bit ）
*  函数返回：无
*  修改时间：2014-9-18
*************************************************************************/
void adc_start(ADCn_Ch_e adcn_ch, ADC_nbit bit)
{
  ADCn_e adcn = (ADCn_e)(adcn_ch >> 5) ;
  uint8_t ch = (uint8)(adcn_ch & 0x1F);
  
  //初始化ADC默认配置
  ADC_CFG1_REG(ADCN[adcn]) = (0
                              //| ADC_CFG1_ADLPC_MASK         //ADC功耗配置，0为正常功耗，1为低功耗
                              | ADC_CFG1_ADIV(2)              //时钟分频选择,分频系数为 2^n,2bit
                              | ADC_CFG1_ADLSMP_MASK          //采样时间配置，0为短采样时间，1 为长采样时间
                              | ADC_CFG1_MODE(bit)
                              | ADC_CFG1_ADICLK(0)            //0为总线时钟,1为总线时钟/2,2为交替时钟（ALTCLK），3为 异步时钟（ADACK）。
                                      );
  
  
  ADC_CFG2_REG(ADCN[adcn])  = (0
                               //| ADC_CFG2_MUXSEL_MASK       //ADC复用选择,0为a通道，1为b通道。
                               //| ADC_CFG2_ADACKEN_MASK      //异步时钟输出使能,0为禁止，1为使能。
                               | ADC_CFG2_ADHSC_MASK          //高速配置,0为正常转换序列，1为高速转换序列
                               | ADC_CFG2_ADLSTS(0)           //长采样时间选择，ADCK为4+n个额外循环，额外循环，0为20，1为12，2为6，3为2
                                   );
  
  //写入 SC1A 启动转换
  ADC_SC1_REG(ADCN[adcn], 0 ) = (0
                                 | ADC_SC1_AIEN_MASK          // 转换完成中断,0为禁止，1为使能
                                   //| ADC_SC1_DIFF_MASK        // 差分模式使能,0为单端，1为差分
                                 | ADC_SC1_ADCH( ch )
                                     );
  
  //ADC_SC1_REG(ADCN[adcn], 1 ) = 0;
}

/*************************************************************************
*                             野火嵌入式开发工作室\蓝宙电子工作室
*
*  函数名称：adc_stop
*  功能说明：停止ADC软件采样
*  参数说明： ADCn_e       ADC模块号（ ADC0、 ADC1）
*  函数返回：无
*  修改时间：2012-2-10
*  备    注：修改苏州大学的例程
*************************************************************************/
void adc_stop(ADCn_e adcn)
{
  ADC_SC1_REG(ADCN[adcn], 0) = (0
                                | ADC_SC1_AIEN_MASK                       // 转换完成中断,0为禁止，1为使能
                                  //| ADC_SC1_DIFF_MASK                     // 差分模式使能,0为单端，1为差分
                                | ADC_SC1_ADCH(Module0_Dis)               //输入通道选择,此处选择禁止通道
                                    );
}



uint_16 adc_deal(int channel, uint8_t accurary, enum HardwareAverage N)
{
  uint8_t ADCCfg = 0;
  uint_16 ADCResult = 0;
  
  switch(accurary)
  {
  case 8:
    ADCCfg = ADC_CFG1_MODE(0);
    break;
  case 12:
    ADCCfg = ADC_CFG1_MODE(1);
    break;
  case 10:
    ADCCfg = ADC_CFG1_MODE(2);
    break;
  case 16:
    ADCCfg = ADC_CFG1_MODE(3);
    break;
  default:
    ADCCfg = ADC_CFG1_MODE(0);		
  }
  ADC0_SC3 = 0 | ADC_SC3_AVGE_MASK | ADC_SC3_AVGS((uint8_t)N);   //SC3寄存器硬件触发
  
  ADCCfg |=  (ADC_CFG1_ADIV(2) | ADC_CFG1_ADICLK(1));
  
  ADC0_CFG1 = ADCCfg;
  
  ADC0_CFG2 = 0 | ADC_CFG2_ADHSC_MASK;
  
  ADC0_SC1A = ADC_SC1_ADCH(channel);
  
  while((ADC0_SC1A & ADC_SC1_COCO_MASK) != ADC_SC1_COCO_MASK)
  {
  }
  
  ADCResult = (uint_16)ADC0_RA;
  
  ADC0_SC1A &= ~ADC_SC1_COCO_MASK;
  
  return ADCResult;
}
