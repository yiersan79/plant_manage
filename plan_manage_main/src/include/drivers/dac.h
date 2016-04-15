//===========================================================================
//文件名称：dac.h
//功能概要：dac底层驱动构件头文件
//版权所有：苏州大学飞思卡尔嵌入式中心(sumcu.suda.edu.cn)
//更新记录：2013-01-12   V1.0
//===========================================================================

#ifndef DAC_H_
#define DAC_H_

#include "common.h"

// 2.2 DAC控制寄存器0 位定义 
#define DAC_DISABLE   0x00                            //禁止DAC
#define DAC_ENABLE    DAC_C0_DACEN_MASK               //使能DAC

#define DAC_SEL_VREFO  0x00                           //参考电压1选择
#define DAC_SEL_VDDA   DAC_C0_DACRFS_MASK             //参考电压2选择

#define DAC_SEL_PDB_HW_TRIG  0x00                     //硬件触发
#define DAC_SEL_SW_TRIG  DAC_C0_DACTRGSEL_MASK        //软件触发

#define DAC_SW_TRIG_STOP 0x00                         //软触发无效
#define DAC_SW_TRIG_NEXT  DAC_C0_DACSWTRG_MASK        //软触发有效

#define DAC_HP_MODE  0x00                             //高功耗模式
#define DAC_LP_MODE  DAC_C0_LPEN_MASK                 //低功耗模式  

//#define DAC_BFWM_INT_DISABLE  0x00                    //禁止缓冲溢出中断
//#define DAC_BFWM_INT_ENABLE   DAC_C0_DACBWIEN_MASK    //使能缓冲溢出中断

#define DAC_BFT_PTR_INT_DISABLE 0x00                  //禁止缓冲读取顶指针溢出
#define DAC_BFT_PTR_INT_ENABLE DAC_C0_DACBTIEN_MASK   //使能缓冲读取顶指针溢出

#define DAC_BFB_PTR_INT_DISABLE 0x00                  //禁止缓冲读取底指针溢出
#define DAC_BFB_PTR_INT_ENABLE DAC_C0_DACBBIEN_MASK   //使能缓冲读取底指针溢出

// 2.3 DAC控制寄存器1 位定义
#define DAC_DMA_DISABLE  0x00                         //禁止DMA
#define DAC_DMA_ENABLE DAC_C1_DMAEN_MASK              //使能DMA
#define DAC_BF_NORMAL_MODE 0x00
//#define 
#define DAC_BF_DISABLE      0x00

//3 函数接口声明

//============================================================================
//函数名称：dac_init
//函数返回：无
//参数说明： VreReference：参考电压选择。0=1.75V，1=3V。
//功能概要：初始化DAC模块设定。
//============================================================================
void dac_init(uint8_t VreReference);

//============================================================================
//函数名称：dac_convert
//函数返回：DAC完成标志。0=完成失败，1=完成成功 
//参数说明：VReference: 参考电压转换值  范围（0~4095）
//功能概要：执行DAC转换。
//============================================================================ 
//uint8_t dac_convert(uint_16 VReference);


//============================================================================
//函数名称：dac_set_buffer
//函数返回：设置的缓冲区大小值
//参数说明：dacx_base_ptr：DACx基指针      
//          dacindex
//          buffval      ：缓冲区值
//功能概要：设置DACx缓冲区
//============================================================================
//int32_t dac_set_buffer(DAC_MemMapPtr dacx_base_ptr, uint8_t dacindex, int32_t buffval);


//============================================================================
//函数名称：dac_set
//函数返回：无
//参数说明：dacx_base_ptr：DACx基指针
//                VrefSel：      参考电压
//功能概要：DAC软件触发   
//============================================================================  
void dac_set(DAC_MemMapPtr dacx_base_ptr, uint8_t VrefSel);

#endif
