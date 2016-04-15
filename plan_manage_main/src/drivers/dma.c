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
#include "common.h"
#include "port.h"
#include "gpio.h"
#include "dma.h"



/*************************************************************************
*                             蓝宙电子工作室
*
*  函数名称：DMA_PORTx2BUFF_Init
*  功能说明：DMA初始化，读取端口数据到内存
*  参数说明：    DMA_CHn         通道号（DMA_CH0 ~ DMA_CH15）
                SADDR           源地址( (void * )&PTx_Bn_IN 或 (void * )&PTx_Wn_IN   )
 *              DADDR           目的地址
 *               PTxn            触发端口
 *               DMA_BYTEn       每次DMA传输字节数
 *             count           一个主循环传输字节数
*  函数返回：无
*  修改时间：2012-2-20
*  * 作者   ：野火工作室/landzo 蓝电子
*  备    注：
*************************************************************************/
void DMA_PORTx2BUFF_Init(DMA_CHn CHn, void *SADDR, void *DADDR, PTxn ptxn, DMA_BYTEn byten, uint32_t count)
{
    uint8_t n, tmp;
    uint8_t BYTEs = (byten == DMA_BYTE1 ? 1 : (byten == DMA_BYTE2 ? 2 : (byten == DMA_BYTE4 ? 4 : 0 ) ) ); //计算传输字节数
    uint8_t ptx0;
    uint32_t temp32 ;
    

    //断言，检测传递进来参数是否正确
    ASSERT(                                             //用断言检测 源地址和每次传输字节数是否正确
        (   (byten == DMA_BYTE1)                    //传输一个字节
            && ( (SADDR >= &PTA_B0_IN) && (SADDR <= ( &PTE_B3_IN )))
        )

        || (   (byten == DMA_BYTE2)                   //传输两个字节(注意，不能跨端口)
               && ( (SADDR >= &PTA_B0_IN)
                    && (SADDR <= ( &PTE_W1_IN ))
                    && (((uint32)SADDR & 0x03) != 0x03) )         //保证不跨端口
           )

        || (   (byten == DMA_BYTE4)                   //传输四个字节
               && ((SADDR >= &PTA_B0_IN) && (SADDR <= ( &PTE_B0_IN )))
               && (((uint32)SADDR & 0x03) == 0x00)           //保证不跨端口
           )
    );

    ASSERT(count < 0x8000); //断言，最大只支持0x7FFF

    //DMA 寄存器 配置

    /* 开启时钟 */
    SIM_SCGC6 |= SIM_SCGC6_DMAMUX_MASK;                     //打开DMA多路复用器时钟
    SIM_SCGC7 |= SIM_SCGC7_DMA_MASK;                        //打开DMA模块时钟



                                       
    DMAMUX0_CHCFG(CHn)=0x00;
    DMA_DSR_BCR(CHn)|=DMA_DSR_BCR_DONE_MASK;

    /* 配置 DMA 通道 的 传输控制块 TCD ( Transfer Control Descriptor ) */
    DMA_SAR(CHn) =    (uint32)SADDR;                      // 设置  源地址
    DMA_DSR_BCR(CHn) =(0
                     | DMA_DSR_BCR_BCR(count)              //传输数目
                     );
    DMA_DAR(CHn) =    (uint32)DADDR;                      // 设置目的地址
    DMA_DCR(CHn) =    (0
                       | DMA_DCR_SSIZE(byten)
                       | DMA_DCR_DSIZE(byten)
                       //| DMA_DCR_SINC_MASK                  //传输后源地址增加(根据位宽)
                       | DMA_DCR_DINC_MASK                  //传输后目的地址增加(根据位宽)
                       | DMA_DCR_CS_MASK                  // 0为不停得传输，直到BCR为0；1为一次请求传输一次
                       //| DMA_DCR_START_MASK               //软件触发传输
                       | DMA_DCR_ERQ_MASK                   //硬件触发传输（与上面START二选一）
                       | DMA_DCR_D_REQ_MASK                 //传输完成后硬件自动清ERQ
                       | DMA_DCR_EADREQ_MASK
                       );



    /* 配置 DMA 触发源 */

    DMAMUX_CHCFG_REG(DMAMUX0_BASE_PTR, CHn) = (0
           // | DMAMUX_CHCFG_ENBL_MASK                        /* Enable routing of DMA request */
            //| DMAMUX_CHCFG_TRIG_MASK                        /* Trigger Mode: Periodic   PIT周期触发传输模式   通道1对应PIT1，必须使能PIT1，且配置相应的PIT定时触发 */
            |DMAMUX_CHCFG_SOURCE(DMA_PORTA)
              //| DMAMUX_CHCFG_SOURCE( PTX(ptxn) + DMA_PORTA) /* 通道触发传输源:     */
                                             );

    //配置触发源（默认是 上升沿触发）
    temp32 =  ALT1 | DMA_FALLING;                                          
    gpio_init(ptxn, GPI, 0);               //设置为输入
    port_init(ptxn, temp32);


    /*  配置输入源   */
    ptx0 = ((((uint32)SADDR) & 0x1C0) / 0x40 ) * 32;
    n = (uint8)(((uint32)SADDR - ((uint32)(&PTA_BYTE0_IN))) & 0x3f) * 8;       //最小的引脚号
    ptx0 += n;
    tmp = ptx0 + (BYTEs * 8 ) - 1;                                          //最大的引脚号
    while(ptx0 <= tmp)
    {
        //这里加入 GPIO 初始化为输入
        gpio_init((PTxn )ptx0, GPI, 0);               //设置为输入
        port_init((PTxn )ptx0 , ALT1 | PULLDOWN );    //输入源应该下拉，默认读取到的是0

        ptx0 ++;
    }

    DMA_DIS(CHn);                                    //禁止通道CHn 硬件请求
    //DMA_IRQ_CLEAN(CHn);
    /* 开启中断 */
    //DMA_EN(CHn);                                    //使能通道CHn 硬件请求
    //DMA_IRQ_EN(CHn);                                //允许DMA通道传输
}
