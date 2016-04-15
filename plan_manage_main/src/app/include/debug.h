/*
 * debug.h - 调试
 */

#ifndef DEBUG_H
#define DEBUG_H


#define REC_DEBUG_UART_LEN  20


enum rec_uart_flag_
{
    A = 0, C, B, D, E, F, G, H, I, J, K, L, M, N, O, P, Q, R, S, T, U, V, W, X, Y, Z
};




extern volatile float recval_debug_uart[26];




/*
 * debug_init() - 初始化调试功能
 */
void debug_init(void);

/*
 * str_to_fp() - 将字符串转换为浮点数
 * @str: 字符串地址
 * @len: 字符串长度
 *
 * 这个函数传入的字符串格式应符合c语言标准浮点格式，并且没有'\0'
 */
float str_to_fp(char str[], uint8_t len);

/*
 * rec_debug_uart() - 接收uart传入的调试信息，并转换为浮点数
 *
 */
void rec_debug_uart(void);









#endif /* DEBUG_H */
