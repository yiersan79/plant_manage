/*
 * knob.c - 提供旋钮输入的接口函数
 *
 * 使用的是一个数字旋钮，利用计数来采集输入
 */

#include <stdint.h>

#include "include.h"


#include "include/knob.h"
#include "include/config.h"



static volatile int32_t knob_val;




/*
 * knob_init() - 为旋钮输入而执行的初始化
 *
 * 旋钮的检测初始化后默认是禁止的
 */
void knob_init(void)
{
    gpio_Interrupt_init(KNOB_A_PINX, GPI_UP_PF, GPI_INP_EITH);
    gpio_Interrupt_init(KNOB_B_PINX, GPI_UP_PF, GPI_DISAB);
    // gpio_Interrupt_init(KNOB_KEY_PINX, GPI_UP_PF, GPI_INP_EITH);
    disable_irq(PortD_irq_no);

}


/*
 * knob_enable() - 使能旋钮脉冲检测
 */
void knob_enable(void)
{
    enable_irq(PortD_irq_no);
    return;
}

/*
 * knob_disable() - 禁止旋钮脉冲检测
 */
void knob_disable(void)
{
    disable_irq(PortA_irq_no);
    return;
}



/*
 * knob_clear() - 清除旋钮值
 */
void knob_clear(void)
{
    knob_val = 0;
    return;
}

/*
 * get_knob_val() - 返回旋钮值
 */
int32_t get_knob_val(void)
{
    return knob_val;
}


/*
 * knob_detect() - 旋钮检测
 *
 * 这个函数需要在旋钮编码器的A相输出发生沿跳变时调用，即上下沿中断时
 */
void knob_detect(void)
{
    uint8_t knob_A, knob_B;

    knob_A = gpio_get(KNOB_A_PINX);
    knob_B = gpio_get(KNOB_B_PINX);
    if ((knob_A == 0 && knob_B == 1) || (knob_A == 1 && knob_B == 0))
    {
        knob_val++;
    }
    else // 即if ((knob_A == 0 && knob_B == 0) || (knob_A == 1 && knob_B == 1))
    {
        knob_val--;
    }
    return;
}
