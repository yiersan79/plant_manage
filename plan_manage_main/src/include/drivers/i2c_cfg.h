/******************** (C) COPYRIGHT 2011 蓝宙电子工作室 ********************
 * 文件名       ：i2c_cfg.h
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
#ifndef _I2C_CFG_H_
#define _I2C_CFG_H_

#include "include.h"

//启动信号
#define i2c_Start(I2Cn)             I2C_C1_REG(I2Cx[I2Cn]) |= (I2C_C1_TX_MASK );\
                                    I2C_C1_REG(I2Cx[I2Cn]) |= ( I2C_C1_MST_MASK)     //MST 由0变1，产生起始信号，TX = 1 进入发送模式

//停止信号
#define i2c_Stop(I2Cn)              I2C_C1_REG(I2Cx[I2Cn]) &= ~(I2C_C1_MST_MASK );\
                                    I2C_C1_REG(I2Cx[I2Cn]) &= ~(I2C_C1_TX_MASK) ;   //MST 由1变0，产生停止信号，TX = 0 进入接收模式

//重复启动
#define i2c_RepeatedStart(I2Cn)     I2C_C1_REG(I2Cx[I2Cn]) |= I2C_C1_RSTA_MASK

//进入接收模式(应答,需要接收多个数据，接收最后一个字节前需要禁用应答i2c_DisableAck)
#define i2c_EnterRxMode(I2Cn)       I2C_C1_REG(I2Cx[I2Cn]) &= ~I2C_C1_TX_MASK;\
                                    I2C_C1_REG(I2Cx[I2Cn]) &= ~I2C_C1_TXAK_MASK
                                      
//进入接收模式(不应答,只接收一个字节)
#define i2c_PutinRxMode(I2Cn)       I2C_C1_REG(I2Cx[I2Cn]) &= ~I2C_C1_TX_MASK
                                    //I2C_C1_REG(I2Cx[I2Cn]) |=  I2C_C1_TXAK_MASK

//禁用应答(接收最后一个字节)
#define i2c_DisableAck(I2Cn)        I2C_C1_REG(I2Cx[I2Cn]) |= I2C_C1_TXAK_MASK

//等待 I2C_S
#define i2c_Wait(I2Cn)              while(( I2C_S_REG(I2Cx[I2Cn]) & I2C_S_IICIF_MASK)==0) {} \
                                    I2C_S_REG(I2Cx[I2Cn]) |= I2C_S_IICIF_MASK;

//判断是否应答了
#define i2c_IsAsk(I2Cn)             (( I2C_S_REG(I2Cx[I2Cn])  & I2C_S_RXAK_MASK)==0 ? 1:0)

//写一个字节
#define i2c_write_byte(I2Cn,data)   (I2C_D_REG(I2Cx[I2Cn]) = (data));



#endif