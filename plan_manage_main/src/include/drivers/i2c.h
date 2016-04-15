/******************** (C) COPYRIGHT 2011 蓝宙电子工作室 ********************
 * 文件名       ：i2c.h
 * 描述         ：I2C头文件
 * 备注         ：修改自官方例程
 *
 * 实验平台     ：
 * 库版本       ：
 * 嵌入系统     ：
 *
 * 作者         ：
 * 淘宝店       ：http://landzo.taobao.com/
**********************************************************************************/
#ifndef I2C_H_
#define I2C_H_

#include "i2c_cfg.h"

/**
 *  @brief I2C模块编号
 */

typedef enum I2Cn
{
    I2C0  = 0,
    I2C1  = 1
} I2Cn;

/**
 *  @brief 主机读写模式选择
 */
typedef enum MSmode
{
    MWSR =   0x00,  /* 主机写模式  */
    MRSW =   0x01   /* 主机读模式  */
} MSmode;

void iic_init(I2Cn i2cn); 

void  I2C_WriteAddr(I2Cn i2cn, uint8_t SlaveID, uint8_t Addr, uint8_t Data);      //读取地址里的内容
uint8_t    I2C_ReadAddr(I2Cn i2cn, uint8_t SlaveID, uint8_t Addr);                //往地址里写入内容
void  I2C_StartTransmission (I2Cn i2cn, uint8_t SlaveID, MSmode);                //启动传输
//uint8_t iic_read1(I2Cn i2cn,uint8_t DeviceAddr, uint8_t AccessAddr, uint8_t *Data);
//uint8_t iic_write1(I2Cn i2cn,uint8_t DeviceAddr, uint8_t AccessAddr, uint8_t Data);
//uint8_t iic_readn(I2Cn i2cn,uint8_t DeviceAddr, uint8_t AccessAddr, uint8_t Data[], uint8_t N);
//uint8_t iic_writen(I2Cn i2cn,uint8_t DeviceAddr, uint8_t AccessAddr,uint8_t Data[], uint8_t N);
//
//uint8_t waitflag(I2Cn i2cn, uint8_t x) ;
//void send_signal(I2Cn i2cn, uint8_t Signal);
#endif /* I2C_H_ */
