/* 
 * 文件：input.h
 * 功能：将外设输入的数据转换为单片机内存中的数据
 */

#ifndef INPUT_H
#define INPUT_H


typedef struct InData_
{
    uint8_t isWifiOn : 1;
    uint8_t isPlanMode : 1;

    uint8_t isRedL : 1;
    uint8_t isBlueL : 1;
    uint8_t isUvbL : 1;

    uint8_t knobV;
} InData;


void InitInput(void);

void 











#endif // INPUT_H
