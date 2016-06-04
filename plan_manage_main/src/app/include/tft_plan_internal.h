/*
 * tft_handle_internal.h - tft和plan_handle模块共同使用部分的内部声明
 *
 * 这个文件只应包含在tft和plan_handle模块
 */

#include "include/pm_time.h"

/*
 * plan_input - 表示计划输入的类型
 * 
 */
typedef struct plan_input_
{
    calendar_info bg_t; // 开始时间
    calendar_info ed_t; // 结束时间
    calendar_info pd_t; // 周期时长

    uint8_t x_orient; // x方向值，未使用
    uint8_t y_orient; // y方向值，未使用
    uint8_t lg_r : 1; // 红灯
    uint8_t lg_b : 1; // 蓝灯
    uint8_t lg_uvb : 1; // uvb
    uint8_t water : 1; // 浇水，未使用
    uint8_t sw : 1; // 是否加入计划
    uint8_t cnt; // 完成次数
} plan_input;

/*
 * kv_pair - 表示页面中的元素
 * key为存储元素ID的字符串，value为一个整形值，attr为元素属性
 */
typedef struct kv_pair_
{
    char *key;
    int16_t value;
    int8_t attr;
} kv_pair;

enum { PLAN_DATA_NUM = 19, OBJ_NUM = 8 };

extern plan_input plan_in[OBJ_NUM];

extern kv_pair kvp_obj_set[][PLAN_DATA_NUM];



void tft_to_plan_input(uint8_t objn);
