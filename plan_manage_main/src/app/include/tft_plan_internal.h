/*
 * tft_handle_internal.h - tft和plan_handle模块共同使用部分的内部声明
 *
 * 这个文件只应包含在tft和plan_handle模块
 */

#include "include/pm_time.h"


typedef struct plan_input_
{
    calendar_info bg_t;
    calendar_info ed_t;
    calendar_info pd_t;

    uint8_t x_orient;
    uint8_t y_orient;
    uint8_t lg_r : 1;
    uint8_t lg_b : 1;
    uint8_t lg_uvb : 1;
    uint8_t water : 1;
    uint8_t sw : 1;
} plan_input;

typedef struct kv_pair_
{
    char *key;
    int16_t value;
    int8_t attr;
} kv_pair;

enum { PLAN_DATA_NUM = 19 };

extern plan_input plan_in[PLAN_DATA_NUM];

extern kv_pair kvp_obj_set[][PLAN_DATA_NUM];



void tft_to_plan_input(uint8_t objn);
