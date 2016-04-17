/*
 * simulat_timer.h - 实现一个模拟定时器
 */

#ifndef SIMULAT_TIMER_H
#define SIMULAT_TIMER_H

#include <stdint.h>

/*
 * 模拟定时器的数量
 */
#define SIMULAT_TIMER_NUM   3



typedef struct st_register_
{
    uint8_t count;      // 定时器计数
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

/*
 * st_init() - 初始化实现模拟定时器所需的pit定时器
 *
 * 基础定时为1ms
 */
void st_base_init(void);


/*
 * st_init() - 初始化一个给定编号的模拟定时器
 * @n: 定时器编号
 * @st_m: 定时模式
 * @cmr_v: 定时器比较值，在非比较模式可以为任意值
 *
 * 这个函数返回0表示正常完成了初始化任务，返回~0表示定时器已打开或不存在
 */
uint8_t st_init(uint8_t n, st_mode st_m, uint32_t cmr_v);


/*
 * st_close() - 关闭一个给定编号的模拟定时器
 * @n: 定时器编号
 */
void st_close(uint8_t n);

/*
 * st_tcf() - 查询tcf位返回
 * @n: 定时器编号
 *
 * 这个函数在没有过比较值的时候返回0，已过比较值得时候返回1，对于不存在或未打开
 */
uint8_t st_tcf(uint8_t n);



/*
 * st_tov() - 查询tov位返回
 * @n: 定时器编号
 *
 * 这个函数在没有溢出时返回0，没有溢出时返回1，对于不存在或未打开
 */
uint8_t st_tov(uint8_t n);



/*
 * st_count() - 查询当前模拟定时值
 * @n: 定时器编号
 *
 * 对于不存在或未打开
 */
uint32_t st_count(uint8_t n);



/********************ISR******************/

/*
 * st_base() -定时调用以模拟N路定时器
 *
 * 定时的最小间隔等于这个函数被调用的周期
 */
void st_base(void);







#endif /* SIMULAT_TIMER_H */
