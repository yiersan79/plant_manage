/*
 * debug.c - 调试功能的实现
 */

#include <stdint.h>

#include "include.h"

#include "include/debug.h"
#include "include/config.h"



volatile float recval_debug_uart[26];


/*
 * debug_init() - 初始化调试功能
 */
void debug_init(void)
{
    uart_init(DEBUG_UARTX, 115200);
    uart_irq_EN(DEBUG_UARTX);
}

/*
 * str_to_fp() - 将字符串转换为浮点数
 * @str: 字符串地址
 * @len: 字符串长度
 *
 * 这个函数传入的字符串格式应符合c语言标准浮点格式，并且没有'\0'
 */
float str_to_fp(char str[], uint8_t len)
{
    uint8_t is_nega = 0;
    uint8_t place_fp = len;
    uint32_t divrsor = 1;
    uint32_t part_int = 0;

    for (uint8_t i = 0; i < len; i++)
    {
        if (str[i] == '-')
            is_nega = 1;
        else if (str[i] == '+')
            is_nega = 0;
        else if (str[i] == '.')
            place_fp = i;
        else
            part_int = part_int * 10 + str[i] - '0';
    }
    for (uint8_t i = place_fp + 1; i < len; i++)
    {
        divrsor *= 10;
    }

    return is_nega ? -(float)part_int / divrsor : (float)part_int / divrsor;
}

/*
 * rec_debug_uart() - 接收uart传入的调试信息，并转换为浮点数
 *
 */
void rec_debug_uart(void)
{
    static char str_log[REC_DEBUG_UART_LEN + 1], index;
    char ch;
    float rec_fp;

    ch = uart_getchar(DEBUG_UARTX);
    if (((ch >= '0' && ch <= '9') || ch == '+' || ch == '-' || ch == '.' )
            && index < REC_DEBUG_UART_LEN)
    {
        str_log[index++] = ch;
    }
    else if ((ch >= 'a' && ch <= 'z') || (ch >= 'A' && ch <= 'Z') || ch == '#')
    {
        rec_fp = str_to_fp(str_log, index);
        switch (ch)
        {
        case 'a':
        case 'A':
            recval_debug_uart[A] = rec_fp;
            break;
        case 'b':
        case 'B':
            recval_debug_uart[B] = rec_fp;
            break;
        case 'c':
        case 'C':
            recval_debug_uart[C] = rec_fp;
            break;
        case 'd':
        case 'D':
            recval_debug_uart[D] = rec_fp;
            break;
        case 'e':
        case 'E':
            recval_debug_uart[E] = rec_fp;
            break;
        case 'f':
        case 'F':
            recval_debug_uart[F] = rec_fp;
            break;
        case 'g':
        case 'G':
            recval_debug_uart[G] = rec_fp;
            break;
        case 'h':
        case 'H':
            recval_debug_uart[H] = rec_fp;
            break;
        case 'i':
        case 'I':
            recval_debug_uart[I] = rec_fp;
            break;
        case 'j':
        case 'J':
            recval_debug_uart[J] = rec_fp;
            break;
        case 'k':
        case 'K':
            recval_debug_uart[K] = rec_fp;
            break;
        case 'l':
        case 'L':
            recval_debug_uart[L] = rec_fp;
            break;
        case 'm':
        case 'M':
            recval_debug_uart[M] = rec_fp;
            break;
        case 'n':
        case 'N':
            recval_debug_uart[N] = rec_fp;
            break;
        case 'o':
        case 'O':
            recval_debug_uart[O] = rec_fp;
            break;
        case 'p':
        case 'P':
            recval_debug_uart[P] = rec_fp;
            break;
        case 'q':
        case 'Q':
            recval_debug_uart[Q] = rec_fp;
            break;
        case 'r':
        case 'R':
            recval_debug_uart[R] = rec_fp;
            break;
        case 's':
        case 'S':
            recval_debug_uart[S] = rec_fp;
            break;
        case 't':
        case 'T':
            recval_debug_uart[T] = rec_fp;
            break;
        case 'u':
        case 'U':
            recval_debug_uart[U] = rec_fp;
            break;
        case 'v':
        case 'V':
            recval_debug_uart[V] = rec_fp;
            break;
        case 'w':
        case 'W':
            recval_debug_uart[W] = rec_fp;
            break;
        case 'x':
        case 'X':
            recval_debug_uart[X] = rec_fp;
            break;
        case 'y':
        case 'Y':
            recval_debug_uart[Y] = rec_fp;
            break;
        case 'z':
        case 'Z':
            recval_debug_uart[Z] = rec_fp;
            break;
        case '#':
            break;
        default:
            break;
        }
        str_log[index] = '\0';
        index = 0;
        if (ch == '#')
        {
            printf("清除缓存成功\n");
        }
        else
        {
            printf("设置%c为%s\n", ch, str_log);
        }
    }
    return;
}
