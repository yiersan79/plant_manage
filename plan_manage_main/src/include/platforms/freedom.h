/*
 * File:        freedom.h
 * Purpose:     Kinetis Freedom hardware definitions
 *
 * Notes:
 */

#ifndef __FREEDOM_H__
#define __FREEDOM_H__

#include "mcg.h"


extern u32 core_clk_khz;
extern u32 core_clk_mhz;
extern u32 bus_clk_khz;

/********************************************************************/

/* Global defines */
#define DEBUG_PRINT

/* Define the CPU used on this hardware platform */
#define CPU_MKL25Z128LK4

#define EXTAL_IN_MHz            (50)

/* Define the default serial port interface */
#define LANDZO_PORT       UART0
#define LANDZO_BAUD       115200
#undef  HW_FLOW_CONTROL
/*************************************************************************
*                            蓝宙电子工作室
*
*  功能说明：定义时钟频率
*  备    注：如果 MCG_CLK_MHZ 定义为 PLLUSR ，则初始化系统时用下面的自定义因子
*************************************************************************/
//#define NO_PLL_INIT
#define K25_CLK             1           // 使用外部时钟参考源
#define REF_CLK             XTAL8       // 没用到，但一些宏定义需要它

#define MCG_CLK_MHZ         PLL48      // 设置时钟频率
#define MAX_FLASH_CLK       30          // flash不能超过25M，这里设为不超过30M



/*********************   自定义 时钟频率 分频因子   ********************/
#include "Fire_kinetis_MCG_CFG.h"   //参考这文件可以 选配 PRDIV、VDIV 。定义MCG分频因子 ：
#define PRDIV             11        // MCG_CLK_MHZ = 50u/(PRDIV+1)*(VDIV+24)
#define VDIV              31
#define CORE_DIV          0         //  core = mcg/ ( CORE_DIV  + 1 )
#define BUS_DIV           0         //  bus  = mcg/ ( BUS_DIV   + 1 )
#define FLEX_DIV          0         //  flex = mcg/ ( FLEX_DIV  + 1 )
#define FLASH_DIV         9         //  flash= mcg/ ( FLASH_DIV + 1 )
/***********************************************************************/
#endif /* __FREEDOM_H__ */
