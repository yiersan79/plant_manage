/*
 * config.h - 配置整个工程
 *
 */

#ifndef CONFIG_H
#define CONFIG_H




/*
 * 模拟定时器使用的定时器
 */
#define SIMULAT_TIMER_PITX      PIT1


/*
 * 调试模块,或8266
 */
#define WIFI_UARTX      UART0   // PTA1,PTA2
#define DEBUG_UARTX     UART0   // PTA1,PTA2

/*
 * 云台
 */
#define ORIENT_UARTX     UART1   // PTC3,PTC4                                                                             
#define RE_DE_PINX       PTC1

/*
 * 输入模块
 */
#define UP_KEY_PINX     PTA4
#define DOWN_KEY_PINX   PTA5
#define OK_KEY_PINX     PTE20
#define RET_KEY_PINX    PTE30
#define AMS_KEY_PINX    PTD5
#define WIFI_KEY_PINX   PTD7
#define MR_KEY_PINX     PTE23
#define MB_KEY_PINX     PTA16
#define MUVB_KEY_PINX   PTD4
// knob用必须是PTA或PTD端口的
#define KNOB_A_PINX     PTD7
#define KNOB_B_PINX     PTD6
#define KNOB_KEY_PINX   PTE20



/*
 * 显示模块
 */
#define TFT_UARTX       UART2  // PTD2,PTD3


/*
 * 时间模块
 */
#define DS1302_CE_PINX      PTB2
#define DS1302_CLK_PINX     PTB1
#define DS1302_IO_PINX      PTB3

/*
 * 控制执行模块
 *
 */
#define LGRED_PINX      PTC17
#define LGBLUE_PINX     PTC16
#define LGUVB_PINX      PTC13
#define WATER_PINX      PTD1

/*
 * led控制，多选几个任意端口
 */
#define LG1_PINX      PTC9
#define LG2_PINX      PTC8
#define LG3_PINX      PTC7
#endif // CONFIG_H
