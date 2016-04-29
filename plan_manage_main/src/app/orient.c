/*
 * orient.c - 关于光照方向控制的模块
 *
 * 所有如上，所有如下
 */


#include "uart.h"

#include "include/orient.h"
#include "include/config.h"


void orient_init(void)
{
    uart_init(ORIENT_UARTX, 2400);
    gpio_init(RE_DE_PINX, 1, 1);
    orient_setmode(0, MODE_MANUL);
    orient_setmode(1, MODE_MANUL);
    return;
}

/*
 * orient_setspeed() - 设置方向控制的速度以及角度
 *
 * @addr: 地址码
 * @ori: 方向
 * @spd: 表示速度，范围在0~100
 */
void orient_setspeed(uint8_t addr, orient ori, uint8_t spd)
{
    uint8_t cmd[7] = { 0xff, addr, 0x00, 0x00, 0x00, 0x00, 0x00 };

    switch (ori)
    {
    case ORIENT_UP:
        cmd[3] = 0x08;
        break;
    case ORIENT_DOWN:
        cmd[3] = 0x10;
        break;
    case ORIENT_LEFT:
        cmd[3] = 0x04;
        break;
    case ORIENT_RIGHT:
        cmd[3] = 0x02;
    default:
        break;
    }

    if (spd > 100)
    {
         spd = 100;
    }
    cmd[4] = 0x3f * spd / 100;
    cmd[6] = cmd[1] + cmd[2] + cmd[3] + cmd[4] + cmd[5];

    uart_sendN(ORIENT_UARTX, cmd, 7);
    return;
}


/*
 * orient_presetop() - 设置预置点
 *
 * @addr: 地址码
 * @op: 对预置点的操作
 * @npre: 预置点编号
 */
void orient_presetop(uint8_t addr, preset_op op, uint8_t npre)
{
    uint8_t cmd[7] = { 0xff, addr, 0x00, 0x00, 0x00, npre, 0x00 };

    switch (op)
    {
    case PRESET_SET:
        cmd[3] = 0x03;
        break;
    case PRESET_CALL:
        cmd[3] = 0x07;
        break;
    case PRESET_CLEAR:
        cmd[3] = 0x05;
        break;
    default:
        break;
    }

    cmd[6] = cmd[1] + cmd[2] + cmd[3] + cmd[4] + cmd[5];
    uart_sendN(ORIENT_UARTX, cmd, 7);
    return;
}

/*
 * orient_setmode() - 设置云台模式
 *
 * @addr: 地址码
 * @mod: 模式标志
 */
void orient_setmode(uint8_t addr, orient_mode mod)
{
    uint8_t cmd[7] = { 0xff, addr, 0x00, 0x00, 0x00, 0x00, 0x00 };

    switch (mod)
    {
    case MODE_AUTO:
        cmd[2] = 0x90;
        break;
    case MODE_MANUL:
        cmd[2] = 0x00;
        break;
    default:
        break;
    }
    cmd[6] = cmd[1] + cmd[2] + cmd[3] + cmd[4] + cmd[5];

    uart_sendN(ORIENT_UARTX, cmd, 7);
    return;
}
