/*
 * plan_handle.c - 计划处理模块
 *
 * 计划处理，指的是根据内存中的计划数据和时间数据得出输出数据，输出数据包括继电器信
 * 号、植物属性值。
 * 关于植物属性值，包括已完成计划周期次数，等。
 * 基本原则是，起始周期时间区间加上重复周期数可以得到一个区间集合，判断当前时间是否
 * 属于这个时间区间集合中的一个区间中的一个时间点。由此得到继电器信号，并在继电器信
 * 号被置为无效的时候将计划周期次数加一。
 * 需要解决的问题有，如何访问计划数据与时间数据，以及如何写入输出数据。
 * 计划数据在tft.c模块，属于静态数据，如果要访问的话，有两种方式，一是声明计划数据
 * 的类型并返回其整体地址，一个整体地址可以访问到所有的对象属性，不过需要知道这个地
 * 址上数据的分布情况，需要额外的数据。二是根据每个元素的名字通过查询得到其单独的值。
 */

#include <stdint.h>

#include "gpio.h"

#include "include/tft_plan_internal.h"

#include "include/plan_handle.h"
#include "include/tft.h"
#include "include/config.h"
#include "include/pm_flash.h"
#include "include/orient.h"
#include "include/key.h"

typedef struct plan_output_
{
    uint8_t is_reach;
    uint8_t note[10];
} plan_output;

typedef enum lg_state_
{
    LIGHT = 1, NO_LIGHT = 0
} lg_state;


static void indata_to_outdata(plan_input *ind, plan_output *outd);
static void plan_ctr_exe(uint8_t activity);
static void plan_inpu_to_tft(void);
static void plan_key_func(void);
static void manul_key_func(void);
static void manul_ctr(void);
static void plan_ctr(void);

static plan_output plan_out[PLAN_DATA_NUM] = { 0 };
plan_input plan_in[PLAN_DATA_NUM] = { 0 };



void plan_handle_init(void)
{
    gpio_init(LGRED_PINX, 1, NO_LIGHT);
    gpio_init(LGBLUE_PINX, 1, NO_LIGHT);
    gpio_init(LGUVB_PINX, 1, NO_LIGHT);
    gpio_init(WATER_PINX, 1, 0);

    gpio_init(LG1_PINX, 1, 1);
    gpio_init(LG2_PINX, 1, 1);
    gpio_init(LG3_PINX, 1, 1);
    // 还有设置方向的初始化

    /*
     * 数据恢复，数据从从flash到plan_handle模块，再从plan_handle模块到tft模块
     */
    flash_read((uint8_t *)&plan_in, sizeof(plan_in));
    plan_inpu_to_tft();

    return;
}



static void indata_to_outdata(plan_input *ind, plan_output *outd)
{
    calendar_info st = get_system_time();
    uint32_t sys_sec = calendar_to_sec(&st);

    ind->pd_t.year = START_YEAR;
    ind->pd_t.month = 1;
    ind->pd_t.mday = 1;
    ind->pd_t.sec = 0;
    uint32_t pd_sec = calendar_to_sec(&ind->pd_t);

    uint32_t ctr_bg_sec = calendar_to_sec(&ind->bg_t) + pd_sec * ind->cnt;
    uint32_t ctr_ed_sec = calendar_to_sec(&ind->ed_t) + pd_sec * ind->cnt;

    if (ctr_bg_sec < sys_sec && ctr_ed_sec > sys_sec)
    {
        outd->is_reach = 1;
    }
    else
    {
        if (outd->is_reach == 1)
        {
            ind->cnt++;
        }
        outd->is_reach = 0;
    }

    return;
}


static void plan_ctr_exe(uint8_t activity)
{
    if (activity >= PLAN_DATA_NUM)
    {
        gpio_set(LGRED_PINX, NO_LIGHT);
        gpio_set(LGBLUE_PINX, NO_LIGHT);
        gpio_set(LGUVB_PINX, NO_LIGHT);
        gpio_set(WATER_PINX, NO_LIGHT);
    }
    else
    {
        orient_presetop(0, PRESET_CALL, activity + 1);
        orient_presetop(1, PRESET_CALL, activity + 1);
        gpio_set(LGRED_PINX, plan_in[activity].lg_r == 1 ? LIGHT : NO_LIGHT);
        gpio_set(LGBLUE_PINX, plan_in[activity].lg_b == 1 ? LIGHT : NO_LIGHT);
        gpio_set(LGUVB_PINX, plan_in[activity].lg_uvb == 1 ? LIGHT : NO_LIGHT);
        gpio_set(WATER_PINX, plan_in[activity].water == 1 ? LIGHT : NO_LIGHT);
    }
    return;
}


void plan_ctr(void)
{
    uint8_t activity = PLAN_DATA_NUM;

    for (uint8_t i = 0; i < PLAN_DATA_NUM; i++)
    {
        indata_to_outdata(&plan_in[i], &plan_out[i]);
        if (plan_in[i].sw == 1)
        {
            if (plan_out[i].is_reach == 1)
            {
                activity = i;
                break;
            }
        }
    }
    plan_ctr_exe(activity);
    return;
}


void plan_handle(void)
{
    if (gpio_get(AMS_KEY_PINX))
    {
        plan_ctr();
    }
    else
    {
        manul_ctr();
    }
    return;
}

void key_func(void)
{
    if (gpio_get(AMS_KEY_PINX))
    {
        plan_key_func();
    }
    else
    {
        manul_key_func();
    }
    return;
}


void manul_ctr(void)
{
    return;
}

/*
 * tft_to_plan_input() - 将tft显示的数据提取到计划处理的输入数据结构中
 * @objn: 提取的对象号，0~7
 *
 * 这个函数应该在tft输入有改变是被调用。
 */
void tft_to_plan_input(uint8_t objn)
{
    plan_in[objn].bg_t.year = *get_value_of_kvp("bg_y", objn);
    plan_in[objn].bg_t.month = *get_value_of_kvp("bg_mo", objn);
    plan_in[objn].bg_t.mday = *get_value_of_kvp("bg_d", objn);
    plan_in[objn].bg_t.hour = *get_value_of_kvp("bg_h", objn);
    plan_in[objn].bg_t.min = *get_value_of_kvp("bg_mi", objn);
    plan_in[objn].bg_t.sec = 0;

    /*
     * 结束时间在tft条目没有年、月、日，赋值为何开始时间相同
     */
    plan_in[objn].ed_t.year = plan_in[objn].bg_t.year;
    plan_in[objn].ed_t.month = plan_in[objn].bg_t.month;
    plan_in[objn].ed_t.mday = plan_in[objn].bg_t.mday;
    plan_in[objn].ed_t.hour = *get_value_of_kvp("ed_h", objn);
    plan_in[objn].ed_t.min = *get_value_of_kvp("ed_mi", objn);
    plan_in[objn].ed_t.sec = 0;

    plan_in[objn].pd_t.mday = *get_value_of_kvp("pd_d", objn);
    plan_in[objn].pd_t.hour = *get_value_of_kvp("pd_h", objn);
    plan_in[objn].pd_t.min = *get_value_of_kvp("pd_mi", objn);

    plan_in[objn].lg_r = *get_value_of_kvp("lg_r", objn);
    plan_in[objn].lg_b = *get_value_of_kvp("lg_b", objn);
    plan_in[objn].lg_uvb = *get_value_of_kvp("lg_uvb", objn);
    plan_in[objn].water = *get_value_of_kvp("water", objn);

    plan_in[objn].sw = *get_value_of_kvp("sw", objn);
    plan_in[objn].cnt = *get_value_of_kvp("cnt", objn);

    return;
}


/*
 * plan_inpu_to_tft() - 计划数据到tft
 *
 * 这个函数是为了在初始化时将从flash中恢复的计划数据重新装入tft
 */
static void plan_inpu_to_tft(void)
{
    for(uint8_t objn = 0; objn < 8; objn++)
    {
        *get_value_of_kvp("bg_y", objn) = plan_in[objn].bg_t.year;
        *get_value_of_kvp("bg_mo", objn) = plan_in[objn].bg_t.month;
        *get_value_of_kvp("bg_d", objn) = plan_in[objn].bg_t.mday;
        *get_value_of_kvp("bg_h", objn) = plan_in[objn].bg_t.hour;
        *get_value_of_kvp("bg_mi", objn) = plan_in[objn].bg_t.min;

        /*
         * 结束时间在tft条目没有年、月、日
         * *get_value_of_kvp("ed_y", objn) = plan_in[objn].ed_t.year;
         * *get_value_of_kvp("ed_mo", objn) = plan_in[objn].ed_t.month;
         * *get_value_of_kvp("ed_d", objn) = plan_in[objn].ed_t.mday;
         */
        *get_value_of_kvp("ed_h", objn) = plan_in[objn].ed_t.hour;
        *get_value_of_kvp("ed_mi", objn) = plan_in[objn].ed_t.min;

        *get_value_of_kvp("pd_d", objn) = plan_in[objn].pd_t.mday;
        *get_value_of_kvp("pd_h", objn) = plan_in[objn].pd_t.hour;
        *get_value_of_kvp("pd_mi", objn) = plan_in[objn].pd_t.min;

        *get_value_of_kvp("lg_r", objn) = plan_in[objn].lg_r;
        *get_value_of_kvp("lg_b", objn) = plan_in[objn].lg_b;
        *get_value_of_kvp("lg_uvb", objn) = plan_in[objn].lg_uvb;
        *get_value_of_kvp("water", objn) = plan_in[objn].water;

        *get_value_of_kvp("sw", objn) = plan_in[objn].sw;
        *get_value_of_kvp("cnt", objn) = plan_in[objn].cnt;
    }
    return;
}


static void plan_key_func(void)
{
    /*
     * 按键功能注册部分
     */
    switch (get_key_mean(UP_KEY))
    {
    case N_KEY:
        //printf("无键\n");
        break;
    case S_KEY:
        printf("UP_KEY 单击\n");
        tft_left();
        break;
    case D_KEY:
        printf("UP_KEY 双击\n");
        tft_up();
        break;
    case L_KEY:
        printf("UP_KEY 长按\n");
        break;
    default:
        break;
    }
    switch (get_key_mean(DOWN_KEY))
    {
    case N_KEY:
        //printf("无键\n");
        break;
    case S_KEY:
        printf("DOWN_KEY 单击\n");
        tft_right();
        break;
    case D_KEY:
        printf("DOWN_KEY 双击\n");
        tft_down();
        break;
    case L_KEY:
        printf("DOWN_KEY 长按\n");
        break;
    default:
        break;
    }
    switch (get_key_mean(OK_KEY))
    {
    case N_KEY:
        //printf("无键\n");
        break;
    case S_KEY:
        printf("OK_KEY 单击\n");
        tft_ok();
        break;
    case D_KEY:
        printf("OK_KEY 双击\n");
        break;
    case L_KEY:
        printf("OK_KEY 长按\n");
        break;
    default:
        break;
    }
    switch (get_key_mean(RET_KEY))
    {
    case N_KEY:
        //printf("无键\n");
        break;
    case S_KEY:
        printf("RET_KEY 单击\n");
        tft_ret();
        break;
    case D_KEY:
        printf("RET_KEY 双击\n");
        break;
    case L_KEY:
        printf("RET_KEY 长按\n");
        break;
    default:
        break;
    }
    switch (get_key_mean(MR_KEY))
    {
    case N_KEY:
        //printf("无键\n");
        break;
    case S_KEY:
        printf("MR_KEY 单击\n");
        break;
    case D_KEY:
        printf("MR_KEY  双击\n");
        break;
    case L_KEY:
        printf("MR_KEY 长按\n");
        break;
    default:
        break;
    }
    switch (get_key_mean(MB_KEY))
    {
    case N_KEY:
        //printf("无键\n");
        break;
    case S_KEY:
        printf("MB_KEY 单击\n");
        break;
    case D_KEY:
        printf("MB_KEY 双击\n");
        break;
    case L_KEY:
        printf("MB_KEY 长按\n");
        break;
    default:
        break;
    }
    switch (get_key_mean(MUVB_KEY))
    {
    case N_KEY:
        //printf("无键\n");
        break;
    case S_KEY:
        printf("MUVB_KEY 单击\n");
        break;
    case D_KEY:
        printf("MUVB_KEY 双击\n");
        break;
    case L_KEY:
        printf("MUVB_KEY 长按\n");
        break;
    default:
        break;
    }
    return;
}


static void manul_key_func(void)
{
    /*
     * 按键功能注册部分
     */
    static uint8_t mlgr, mlgb, mlguvb;

    switch (get_key_mean(UP_KEY))
    {
    case N_KEY:
        //printf("无键\n");
        break;
    case S_KEY:
        printf("UP_KEY 单击\n");
        orient_setspeed(0, ORIENT_LEFT, 100);
        break;
    case D_KEY:
        printf("UP_KEY 双击\n");
        orient_setspeed(0, ORIENT_RIGHT, 100);
        break;
    case L_KEY:
        printf("UP_KEY 长按\n");
        break;
    default:
        break;
    }
    switch (get_key_mean(DOWN_KEY))
    {
    case N_KEY:
        //printf("无键\n");
        break;
    case S_KEY:
        printf("DOWN_KEY 单击\n");
        orient_setspeed(1, ORIENT_LEFT, 100);
        break;
    case D_KEY:
        printf("DOWN_KEY 双击\n");
        orient_setspeed(1, ORIENT_RIGHT, 100);
        break;
    case L_KEY:
        printf("DOWN_KEY 长按\n");
        break;
    default:
        break;
    }
    switch (get_key_mean(OK_KEY))
    {
    case N_KEY:
        //printf("无键\n");
        break;
    case S_KEY:
        printf("OK_KEY 单击\n");
        break;
    case D_KEY:
        printf("OK_KEY 双击\n");
        break;
    case L_KEY:
        printf("OK_KEY 长按\n");
        break;
    default:
        break;
    }
    switch (get_key_mean(RET_KEY))
    {
    case N_KEY:
        //printf("无键\n");
        break;
    case S_KEY:
        printf("RET_KEY 单击\n");
        orient_setmode(0, MODE_MANUL);
        orient_setmode(1, MODE_MANUL);
        break;
    case D_KEY:
        printf("RET_KEY 双击\n");
        break;
    case L_KEY:
        printf("RET_KEY 长按\n");
        break;
    default:
        break;
    }
    switch (get_key_mean(MR_KEY))
    {
    case N_KEY:
        //printf("无键\n");
        break;
    case S_KEY:
        printf("MR_KEY 单击\n");
        mlgr = !mlgr;
        gpio_set(LGRED_PINX, mlgr == 1 ? LIGHT : NO_LIGHT);
        break;
    case D_KEY:
        printf("MR_KEY  双击\n");
        break;
    case L_KEY:
        printf("MR_KEY 长按\n");
        break;
    default:
        break;
    }
    switch (get_key_mean(MB_KEY))
    {
    case N_KEY:
        //printf("无键\n");
        break;
    case S_KEY:
        printf("MB_KEY 单击\n");
        mlgb = !mlgb;
        gpio_set(LGBLUE_PINX, mlgb == 1 ? LIGHT : NO_LIGHT);
        break;
    case D_KEY:
        printf("MB_KEY 双击\n");
        break;
    case L_KEY:
        printf("MB_KEY 长按\n");
        break;
    default:
        break;
    }
    switch (get_key_mean(MUVB_KEY))
    {
    case N_KEY:
        //printf("无键\n");
        break;
    case S_KEY:
        printf("MUVB_KEY 单击\n");
        mlguvb = !mlguvb;
        gpio_set(LGUVB_PINX, mlguvb == 1 ? LIGHT : NO_LIGHT);
        break;
    case D_KEY:
        printf("MUVB_KEY 双击\n");
        break;
    case L_KEY:
        printf("MUVB_KEY 长按\n");
        break;
    default:
        break;
    }
    return;
}
