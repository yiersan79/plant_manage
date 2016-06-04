/*
 * knob.h - 提供旋钮输入的接口函数
 *
 * 使用的是一个数字旋钮，利用计数来采集输入
 */

#ifndef KNOB_H
#define KNOB_H






void knob_init(void);


void knob_enable(void);

void knob_disable(void);

void knob_clear(void);

int32_t get_knob_val(void);

void knob_detect(void);


#endif /* KNOB_H */
