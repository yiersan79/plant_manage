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

#include  "i2c.h"


volatile struct I2C_MemMap *I2Cx[2] = {I2C0_BASE_PTR, I2C1_BASE_PTR}; //定义两个指针数组保存 I2Cx 的地址



//============================================================================
//函数名称：iic_init
//功能概要：初始化IICX模块。
//参数说明：MoudleNum:模块号
//函数返回：无
//============================================================================
void iic_init(I2Cn i2cn)
{
  uint8_t mult;

  if(i2cn==I2C0)
  {
    SIM_SCGC4 |=SIM_SCGC4_I2C0_MASK;
    /* 配置 I2C0功能的 GPIO 接口 */
    if((I2C0_SCL_PIN == PTB0) || (I2C0_SCL_PIN == PTB2) ||I2C0_SCL_PIN == PTC8 )
      port_init (I2C0_SCL_PIN, ALT2 | PULLUP );
    else
      ASSERT(0);                              //上诉条件都不满足，直接断言失败了，设置管脚有误？
    
    if ((I2C0_SDA_PIN == PTB1) ||  (I2C0_SDA_PIN == PTB3) ||I2C0_SDA_PIN == PTC9 )
      port_init (I2C0_SDA_PIN, ALT2 | PULLUP );
    else
      ASSERT(0); 
    
    
    I2C0_C1=0X00;
    I2C0_S=I2C_S_IICIF_MASK;
    
  }
  else
  {
    
    //sclk =  core_clk_khz * 1000;

    SIM_SCGC4 |=SIM_SCGC4_I2C1_MASK;
    /* 配置 I2C1功能的 GPIO 接口 */
    if(I2C1_SCL_PIN == PTE1)
      port_init (I2C1_SCL_PIN, ALT6 | PULLUP );
    else if(I2C1_SCL_PIN == PTC10)
      port_init (I2C1_SCL_PIN, ALT2 | PULLUP );
    else
      ASSERT(0);                          //上诉条件都不满足，直接断言失败了，设置管脚有误？
    
    if(I2C1_SDA_PIN == PTE0)
      port_init (I2C1_SDA_PIN, ALT6 | PULLUP );
    else if (I2C1_SDA_PIN == PTC11)
      port_init (I2C1_SDA_PIN, ALT2 | PULLUP );
    else
      ASSERT(0);
    
    I2C1_C1=0X00;
    I2C1_S=I2C_S_IICIF_MASK;
    
  }
  
  
  /* 设置频率 */
  if(bus_clk_khz <= 24*1000)mult = 0;         //bus 一分频
  else  if(bus_clk_khz <=  48*1000)mult = 1;  //bus 二分频
  else      mult = 2;                       //bus 四分频
  

  I2C_F_REG(I2Cx[i2cn])  = ( 0                        // I2C Frequency Divider register (I2Cx_F)  I2C分频寄存器   I2C最大波特率为 400k
                            | I2C_F_MULT(mult)        // 乘数因子 mul = MULT + 1
                            | I2C_F_ICR(17)          // 时钟速率 = ICR_2_SCL_divider[ICR] ，查表获得 ICR 与 SCL_divider 映射关系
                            );

  
  /* 使能 IIC1 */
  I2C_C1_REG(I2Cx[i2cn]) = I2C_C1_IICEN_MASK  ;     //使能I2C
  
  
  
}

/*************************************************************************
*                             蓝宙电子工作室
*
*  函数名称：I2C_StartTransmission
*  功能说明：启动 I2C 传输
*  参数说明：I2Cn        模块号（I2C0、I2C1）
*            SlaveID     7位从机地址
*            MSmode      读写模式（ MWSR 或 MRSW ）
*  函数返回：无
*  修改时间：2012-1-20
*  备    注：
*************************************************************************/
void I2C_StartTransmission (I2Cn i2cn, uint8_t SlaveID, MSmode Mode)
{

    //ASSERT(Mode == MWSR || Mode == MRSW);         //使用断言，检测 Mode 是否为 读 或 写

    SlaveID = ( SlaveID << 1 ) | Mode ;            //确定写地址和读地址

    /* send start signal */
    i2c_Start(i2cn);

    /* send ID with W/R bit */
    i2c_write_byte(i2cn, SlaveID);
}

/*************************************************************************
*                             蓝宙电子工作室
*
*  函数名称：Pause
*  功能说明：延时，I2C读写后，需要延时一下
*  参数说明：无
*  函数返回：无
*  修改时间：2012-1-20
*  备    注：在官方基础上增大延时数
*************************************************************************/
void Pause(void)
{
    uint16_t n;
    for(n = 1; n < 20000; n++)      //注意，这个数据太小，会导致读取错误。
    {
        asm("nop");
    }
}

/*************************************************************************
*                             蓝宙电子工作室
*
*  函数名称：I2C_ReadAddr
*  功能说明：读取I2C设备指定地址寄存器的数据
*  参数说明：I2Cn        模块号（I2C0、I2C1）
*            SlaveID     7位从机地址
*            Addr        从机的寄存器地址
*  函数返回：从机寄存器的数据
*  修改时间：2012-1-20
*  备    注：
*************************************************************************/
uint8_t I2C_ReadAddr(I2Cn i2cn, uint8_t SlaveID, uint8_t Addr)
{
    uint8_t result;

    /* Send Slave Address */
    I2C_StartTransmission (i2cn, SlaveID, MWSR);
    i2c_Wait(i2cn);

    /* Write Register Address */
    i2c_write_byte(i2cn, Addr);
    i2c_Wait(i2cn);

    /* Do a repeated start */
    i2c_RepeatedStart(i2cn);

    /* Send Slave Address */
    i2c_write_byte(i2cn, ( SlaveID << 1) | MRSW );
    i2c_Wait(i2cn);

    /* Put in Rx Mode */
    i2c_PutinRxMode(i2cn);

    /* Turn off ACK since this is second to last byte being read*/
    i2c_DisableAck(i2cn); //不应答

    /* Dummy read 虚假读取*/
    result = I2C_D_REG(I2Cx[i2cn]);
    i2c_Wait(i2cn);

    /* Send stop since about to read last byte */
    i2c_Stop(i2cn);

    /* Read byte */
    result = I2C_D_REG(I2Cx[i2cn]);

    return result;
}


/*************************************************************************
*                             蓝宙电子工作室
*
*  函数名称：I2C_WriteAddr
*  功能说明：写入一个字节数据到I2C设备指定寄存器地址
*  参数说明：I2Cn        模块号（I2C0、I2C1）
*            SlaveID     7位从机地址
*            Addr        从机的寄存器地址
*            Data        数据
*  函数返回：无
*  修改时间：2012-1-20
*  备    注：
*************************************************************************/
void I2C_WriteAddr(I2Cn i2cn, uint8_t SlaveID, uint8_t Addr, uint8_t Data)
{
    /* send data to slave */
    I2C_StartTransmission(i2cn, SlaveID, MWSR);    //启动传输
    i2c_Wait(i2cn);

    i2c_write_byte(i2cn, Addr);                    //写地址
    i2c_Wait(i2cn);

    i2c_write_byte(i2cn, Data);                    //写数据
    i2c_Wait(i2cn);

    i2c_Stop(i2cn);

    Pause();                                        //延时太短的话，可能写出错
}

////============================================================================
////函数名称：send_signal
////功能概要：根据需要产生开始或停止信号   。
////参数说明：  i2cn:模块号
////          Signal: 'S'(Start),产生开始信号      'O'(Over),  产生停止信号
////函数返回：无
////============================================================================
//void send_signal(I2Cn i2cn, uint8_t Signal)
//{
//
//  if(i2cn==I2C0)
//  {
//    if (Signal == 'S')
//    {
//      //i2c0_c主机模式选择位MST由0变为1,可以产生开始信号
//      I2C0_C1 |=0x20;
//    }
//    else if (Signal == 'O')
//    {
//      //主机模式选择位MST由1变为0,可以产生停止信号
//      I2C0_C1 &=0xdf;
//    }
//  }
//  else if(i2cn==I2C1)
//  {
//    if (Signal == 'S')
//    {
//      //i2c0_c主机模式选择位MST由0变为1,可以产生开始信号
//      I2C1_C1 |=0x20;
//    }
//    else if (Signal == 'O')
//    {
//      //主机模式选择位MST由1变为0,可以产生停止信号
//      I2C1_C1 &=0xdf;
//    }
//  }
//}
////============================================================================
////函数名称：waitflag
////功能概要： 在时限内,循环检测接收应答标志位,或传送完成标志位,判断MCU
////          是否接收到应答信号或一个字节是否已在总线上传送完毕
////参数说明：         i2cn:模块号
////             x:x = 'A'(Ack),等待应答;x = 'T'(Transmission),等待一个字节
////                 数据传输完成
////函数返回：0:收到应答信号或一个字节传送完毕;
////         1:未收到应答信号或一个 字节没传送完
////============================================================================
//
//uint8_t waitflag(I2Cn i2cn, uint8_t x)
//
//{
//  uint16_t ErrTime, i;
//
//  ErrTime = 255*10;            //定义查询超时时限
//  for (i = 0;i < ErrTime;i++)
//  {
//    if (x == 'A')           //等待应答信号
//    {
//      if(( I2C_S_REG(I2Cx[i2cn])  & I2C_S_RXAK_MASK)==0)
//        return 0;      //传送完一个字节后,收到了从机的应答信号
//    }
//    else if (x == 'T')      //等待传送完成一个字节信号
//    {
//      if ((I2C_S_REG(I2Cx[i2cn]) & I2C_S_IICIF_MASK) != 0)
//      {
//        (I2C_S_REG(I2Cx[i2cn]) |=(0 | I2C_S_IICIF_MASK));  //清IICIF标志位
//        return 0;       //成功发送完一个字节
//      }
//    }
//  }
//  if (i >= ErrTime)
//    return 1;               //超时,没有收到应答信号或发送完一个字节
//  return 0; 
//}
////============================================================================
////函数名称：iic_read1
////功能概要：从从机读1个字节数据
////参数说明：  i2cn:模块号
////          DeviceAddr:设备地址
////          AccessAddr:访问地址
////          Data:带回收到的一个字节数据
////函数返回：为0,成功读一个字节;为1,读一个字节失败
////函数说明: 内部调用 send_signal,wait
////============================================================================
//uint8_t iic_read1(I2Cn i2cn,uint8_t DeviceAddr, uint8_t AccessAddr, uint8_t *Data)
//{
//  
//  I2C_C1_REG(I2Cx[i2cn])     |= 0x10;           //TX = 1,MCU设置为发送模式
//  send_signal(i2cn, 'S');               //发送开始信号
//  I2C_D_REG(I2Cx[i2cn])  = DeviceAddr & 0xfe;   //发送设备地址,并通知从机接收数据
//  
//  if (waitflag(i2cn, 'T'))                  //等待一个字节数据传送完成
//  {
//    
//    return 1;                        //没有传送成功,读一个字节失败
//  }
//  if (waitflag(i2cn, 'A'))                 //等待从机应答信号
//  {
//    
//    return 1;                         //没有等到应答信号,读一个字节失败
//  }
//  I2C_D_REG(I2Cx[i2cn])  = AccessAddr;        //发送访问地址
//  if (waitflag(i2cn, 'T'))                //等待一个字节数据传送完成
//  {
//    
//    return 1;                        //没有传送成功,读一个字节失败
//  }
//  if (waitflag(i2cn, 'A'))                //等待从机应答信号
//  {
//    
//    return 1;                        //没有等到应答信号,读一个字节失败
//  }
//  I2C_C1_REG(I2Cx[i2cn]) |= 0x04;//当MCU在主机模 式下，向该位写1将产生一个重新开始信号
//  I2C_D_REG(I2Cx[i2cn]) = DeviceAddr | 0x01; //通知从机改为发送数据
//  if (waitflag(i2cn, 'T'))               //等待一个字节数据传送完成
//  {
//    
//    return 1;                       //没有传送成功,读一个字节失败
//  }
//  if (waitflag(i2cn, 'A'))               //等待从机应答信号
//  {
//    
//    return 1;                      //没有等到应答信号,读一个字节失败
//  }
//  I2C_C1_REG(I2Cx[i2cn]) &= 0xef;           //TX = 0,MCU设置为接收模式
//  *Data = I2C_D_REG(I2Cx[i2cn]);            //读出IIC1D,准备接收数据
//  if (waitflag(i2cn, 'T'))              //等待一个字节数据传送完成
//  {
//    
//    return 1;                      //没有传送成功,读一个字节失败
//  }
//  send_signal(i2cn, 'O');           //发送停止信号
//  *Data = I2C_D_REG(I2Cx[i2cn]);            //读出接收到的一个数据
//  
//  return 0;                          //正确接收到一个字节数据
//  
//  
//}
//
////============================================================================
////函数名称：iic_write1
////功能概要：向从机写1个字节数据
////参数说明：  i2cn:模块号
////          DeviceAddr:设备地址
////          AccessAddr:访问地址
////          Data:要发给从机的1个字节数据
////函数返回：为0,成功写一个字节;为1,写一个字节失败
////函数说明: 内部调用 send_signal,wait
////============================================================================
//
//uint8_t iic_write1(I2Cn i2cn,uint8_t DeviceAddr, uint8_t AccessAddr, uint8_t Data)
//{
//  
//  I2C_C1_REG(I2Cx[i2cn]) |= 0x10;             //TX = 1,MCU设置为发送模式
//  send_signal(i2cn, 'S');             //发送开始信号
//  I2C_D_REG(I2Cx[i2cn]) = DeviceAddr & 0xfe;  //发送设备地址,并通知从机接收数据
//  if (waitflag('T',i2cn))                //等待一个字节数据传送完成
//    return 1;                        //没有传送成功,写一个字节失败
//  if (waitflag('A',i2cn))               //等待从机应答信号
//    return 1;                       //没有等到应答信号,写一个字节失败
//  I2C_D_REG(I2Cx[i2cn]) = AccessAddr;         //发送访问地址
//  if (waitflag('T',i2cn))               //等待一个字节数据传送完成
//    return 1;                       //没有传送成功,写一个字节失败
//  if (waitflag('A',i2cn))               //等待从机应答信号
//    return 1;                      //没有等到应答信号,写一个字节失败
//  I2C_D_REG(I2Cx[i2cn]) = Data;             //写数据
//  if (waitflag('T',i2cn))              //等待一个字节数据传送完成
//    return 1;                      //没有传送成功,写一个字节失败
//  if (waitflag('A',i2cn))              //等待从机应答信号
//    return 1;                      //没有等到应答信号,写一个字节失败
//  send_signal('O',i2cn);           //发送停止信号
//  return 0;
//}
////============================================================================
////函数名称： iic_readn
////功能概要： 从从机读N个字节数据
////参数说明：  i2cn:模块号
////          DeviceAddr:设备地址
////          AccessAddr:访问地址
////          Data:读出数据的缓冲区
////          N:从从机读的字节个数
////函数返回： 为0,成功读N个字节;为1,读N个字节失败
////函数说明: 内部调用iic_read1
////============================================================================
//uint8_t iic_readn(I2Cn i2cn,uint8_t DeviceAddr, uint8_t AccessAddr, uint8_t Data[], uint8_t N)
//{
//  uint16_t i, j;
//  for (i = 0;i < N;i++)
//  {
//    for(j = 0;j <15*40 ;j++); //最小延时(发送的每个字节之间要有时间间隔)
//    if (iic_read1(i2cn,DeviceAddr, AccessAddr + i, &Data[i]))
//      return 1;             //其中一个字节没有接收到,返回失败标志:1
//  }
//  if (i >= N)
//    return 0;                 //成功接收N个数据,返回成功标志:0
//}
//
//
////============================================================================
////函数名称： iic_writen
////功能概要： 向从机写N个字节数据
////参数说明：  i2cn:模块号
////          DeviceAddr:设备地址
////          AccessAddr:访问地址
////          Data:要写入的数据
////          N:从从机读的字节个数
////函数返回： 为0,成功写N个字节;为1,写N个字节失败
////函数说明: 内部调用iic_write1
////============================================================================
//uint8_t iic_writen(I2Cn i2cn,uint8_t DeviceAddr, uint8_t AccessAddr,uint8_t Data[], uint8_t N)
//{
//  uint16_t i, j;
//  for (i = 0;i < N;i++)
//  {
//    for(j = 0;j < 15*40;j++); //最小延时(发送的每个字节之间要有时间间隔)
//    if (iic_write1(i2cn,DeviceAddr, AccessAddr + i, Data[i]))
//      return 1;            //其中一个字节没有发送出去,返回失败标志:1
//  }
//  if (i >= N)
//    return 0;                //成功发送N个数据,返回成功标志:0
//}
//
