/*
 * knob.h - 提供旋钮输入的接口函数
 *
 * 使用的是一个数字旋钮，利用计数来采集输入
 */

#ifndef KNOB_H
#define KNOB_H

#include "arm_cm0.h"
#include "vectors.h"




/*
 * knob_init() - 为旋钮输入而执行的初始化
 *
 * 旋钮的检测初始化后默认是禁止的
 */
void knob_init(void);



/*
 * knob_enable() - 使能旋钮脉冲检测
 */
void knob_enable(void);


/*
 * knob_disable() - 禁止旋钮脉冲检测
 */
void knob_disable(void);


/*
 * knob_clear() - 清除旋钮值
 */
void knob_clear(void);

/*
 * get_knob_val() - 返回旋钮值
 */
int32_t get_knob_val(void);


/*
 * knob_detect() - 旋钮检测
 *
 * 这个函数需要在旋钮编码器的A相输出发生沿跳变时调用，即上下沿中断时
 */
void knob_detect(void);


#endif /* KNOB_H */
