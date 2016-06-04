/*
 * simulat_timer.h - 实现一个模拟定时器
 */

#ifndef SIMULAT_TIMER_H
#define SIMULAT_TIMER_H

#include <stdint.h>

/*
 * 模拟定时器的数量
 */
#define SIMULAT_TIMER_NUM   5



typedef struct st_register_
{
    uint32_t count;      // 定时器计数
    uint32_t cmr;       // 比较寄存器
    struct
    {
        uint8_t ten : 1;    // 使能标志
        uint8_t tov : 1;    // 溢出标志
        uint8_t tfc : 1;    // 自由运行标志
        uint8_t tcf : 1;    // 比较标志
    } flag;
} st_register;


typedef enum st_mode_
{
    COMPARE = 0,
    NO_COMPARE
} st_mode;


void st_base_init(void);
uint8_t st_init(uint8_t n, st_mode st_m, uint32_t cmr_v);
void st_close(uint8_t n);
uint8_t st_tcf(uint8_t n);
uint8_t st_tov(uint8_t n);
uint32_t st_count(uint8_t n);
void st_base(void);







#endif /* SIMULAT_TIMER_H */
