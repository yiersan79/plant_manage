/******************** (C) COPYRIGHT 2011 蓝宙电子工作室 ********************
 * 文件名       ：fire_drivers_cfg.h
 * 描述         ：K60复用管家配置
 * 备注         ：，，
 *
 * 实验平台     ：
 * 库版本       ：
 * 嵌入系统     ：
 *
 * 作者         ：蓝宙电子工作室
 * 淘宝店       ：http://landzo.taobao.com/
**********************************************************************************/

#ifndef _FIRE_DRIVERS_CFG_H_
#define _FIRE_DRIVERS_CFG_H_

#include "gpio_cfg.h"


/**********************************  UART   ***************************************/

//      模块通道    端口          可选范围                          建议
#define UART0_RX    PTA1       //PTE21、PTA1、PTA15、PTB16、PTD6          
#define UART0_TX    PTA2       //PTE20、PTA2、PTA14、PTB17、PTD7          

#define UART1_RX    PTC3        //PTC3、PTE1
#define UART1_TX    PTC4        //PTC4、PTE0

#define UART2_RX    PTD2        //PTD2 、PTD4
#define UART2_TX    PTD3        //PTD3 、PTD5

/**********************************  TPM    ***************************************/

#define TPM_CLKIN0_PIN  PTC12       //PTA18、PTB16、PTC12、PTE16、PTE29     PTA18不要用 （与晶振冲突）
#define TPM_CLKIN1_PIN  PTC13       //PTA19、PTB17、PTC13、PTE17、PTE30              PTA19不要用 （与晶振冲突）


//      模块通道    端口          可选范围                                    建议
#define TPM0_CH0_PIN    PTE24        //PTE24、PTA3、PTC1、PTD0                PTA3不要用（与SWD冲突）
#define TPM0_CH1_PIN    PTA4        //PTE25、PTA4、PTC2、PTD1
#define TPM0_CH2_PIN    PTA5        //PTE29、PTA5、PTC3、PTD2
#define TPM0_CH3_PIN    PTC4       //PTE30、PTA6、PTC4、PTD3
#define TPM0_CH4_PIN    PTC8       //PTE31、PTA7、PTC8、PTD4
#define TPM0_CH5_PIN    PTC9        //PTE26、PTA0、PTC9、PTD5                 PTA0不要用（与SWD冲突）


//      模块通道    端口          可选范围                                    建议
#define TPM1_CH0_PIN    PTA12       //PTA12、PTB0、PTE20
#define TPM1_CH1_PIN    PTA13       //PTA13、PTB1、PTE21

//      模块通道    端口          可选范围                                    建议
#define TPM2_CH0_PIN    PTB18       //PTA1、PTB2、PTB18、PTE22
#define TPM2_CH1_PIN    PTA2      //PTA2、PTB3、PTB19、PTE23



/**********************************  I2C   ***************************************/

//      模块通道    端口          可选范围                                    建议
#define I2C0_SCL_PIN    PTB2        // PTB0、PTB2、PTC8、PTE19、PTE24
#define I2C0_SDA_PIN    PTB3        // PTB1、PTB3、PTC9、PTE18、PTE25

#define I2C1_SCL_PIN    PTC10       // PTE1、PTA3、PTC1、PTC10                     PTA3不要用（与SWD冲突）
#define I2C1_SDA_PIN    PTC11       // PTE0、PTA4、PTC2、PTC11


/**********************************  SPI   ***************************************/
//PCS接口，不用的时候需要注释，就不会进行初始化对应的管脚
//      模块通道    端口          可选范围                  建议

#define SPI0_SCK_PIN    PTA15        // PTE17、PTA15、PTC5、PTD1
#define SPI0_SOUT_PIN   PTC7        // PTE18、PTE19、PTA16、PTA17、PTC6、PTC7、PTD2、PTD3
#define SPI0_SIN_PIN    PTC6        // PTE18、PTE19、PTA16、PTA17、PTC6、PTC7、PTD2、PTD3

#define SPI0_PCS0_PIN   PTC4        // PTE16、PTA14、PTC4、PTD0



#define SPI1_SCK_PIN    PTE2        // PTB9、PTB11、PTD5、PTE2
#define SPI1_SOUT_PIN   PTE1        // PTB16、PTB17、PTD6、PTD7、PTE1、PTE3
#define SPI1_SIN_PIN    PTE3        // PTB16、PTB17、PTD6、PTD7、PTE1、PTE3、PTE0

#define SPI1_PCS0_PIN   PTE4        // PTB8、PTB10、PTE4

/**********************************  DAC   ***************************************/
#define DAC0_OUT_PIN    PTE30       // PTE30

#endif  //_FIRE_DRIVERS_CFG_H_


