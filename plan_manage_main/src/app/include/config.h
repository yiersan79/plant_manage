/* 
 * config.h - 配置整个工程
 *
 */

#ifndef CONFIG_H
#define CONFIG_H




/*
 * 模拟定时器使用的定时器
 */
#define SIMULAT_TIMER_PITX		PIT1


/*
 * 调试模块
 */
#define DEBUG_UARTX		UART1



/*
 * 输入模块
 */
#define UP_KEY_PINX     PTE20
#define DOWN_KEY_PINX   PTA17
#define OK_KEY_PINX     PTE23
#define RET_KEY_PINX    PTA4
#define AMS_KEY_PINX    PTA5
#define WIFI_KEY_PINX   PTD7
#define MR_KEY_PINX     PTE30
#define MB_KEY_PINX     PTA16
#define MUVB_KEY_PINX   PTD6

#define KNOB_A_PINX     PTD6
#define KNOB_B_PINX     PTD7
#define KNOB_KEY_PINX   PTD4    // PTD5



/*
 * 显示模块
 */
#define TFT_UARTX       UART2  // PTD2,PTD3




#endif // CONFIG_H
