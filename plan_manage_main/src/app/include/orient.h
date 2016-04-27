/*
 * orient.h - 方向控制
 *
 * 所有如上，所有如下
 */


#ifndef ORIENT_H
#define ORIENT_H


#include <stdint.h>




typedef enum orient_
{
    ORIENT_UP = 0, ORIENT_DOWN, ORIENT_RIGHT, ORIENT_LEFT
} orient;

typedef enum preset_op_
{
    PRESET_SET = 0, PRESET_CLEAR, PRESET_CALL
} preset_op;

typedef enum orient_mode_
{
    MODE_AUTO = 0, MODE_MANUL
} orient_mode;



void orient_init(void);

void orient_setspeed(uint8_t addr, orient ori, uint8_t spd);

void orient_setpreset(uint8_t addr, preset_op op, uint8_t npre);

void orient_setmode(uint8_t addr, orient_mode mod);









#endif /* ORIENT_H */

