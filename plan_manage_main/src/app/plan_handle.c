/*
 * plan_handle.c - 计划处理模块
 *
 * 计划处理，指的是根据内存中的计划数据和时间数据得出输出数据，输出数据包括继电
 * 器信号、植物属性值。
 * 关于植物属性值，包括已完成计划周期次数，等。
 * 基本原则是，起始周期时间区间加上重复周期数可以得到一个区间集合，判断当前时间
 * 是否属于这个时间区间集合中的一个区间中的一个时间点。由此得到继电器信号，并在
 * 继电器信号被置为无效的时候将计划周期次数加一。
 * 需要解决的问题有，如何访问计划数据与时间数据，以及如何写入输出数据。
 * 计划数据在tft.c模块，属于静态数据，如果要访问的话，有两种方式，一是声明计划
 * 数据的类型并返回其整体地址，一个整体地址可以访问到所有的对象属性，不过需要知
 * 道这个地址上数据的分布情况，需要额外的数据。二是根据每个元素的名字通过查询得
 * 到其单独的值。
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

/*
 * 计划输出数据类型定义
 * is_reach表示是否到达计划执行时间，
 * note是一个字符串，可以用来存储一些提示性字符
 */
typedef struct plan_output_
{
    uint8_t is_reach;
    uint8_t note[10];
} plan_output;

typedef enum lg_state_
{
    LIGHT = 1, NO_LIGHT = 0
} lg_state;


/*
 * 静态函数声明
 */
static void indata_to_outdata(plan_input *ind, plan_output *outd);
static void plan_ctr_exe(uint8_t activity);
static void plan_inpu_to_tft(void);
static void plan_key_func(void);
static void manul_key_func(void);
static void manul_ctr(void);
static void plan_ctr(void);

/*
 * 输出和输入变量定义，初始化全部为0
 */
static plan_output plan_out[OBJ_NUM] = { 0 };
plan_input plan_in[OBJ_NUM] = { 0 };



/**
 * plan_handle_init() - 对计划处理模块进行初始化
 *
 * 主要包括IO初始化和计划数据的上电恢复工作
 */
void plan_handle_init(void)
{
    gpio_init(LGRED_PINX, 1, NO_LIGHT);
    gpio_init(LGBLUE_PINX, 1, NO_LIGHT);
    gpio_init(LGUVB_PINX, 1, NO_LIGHT);
    gpio_init(WATER_PINX, 1, 0);

    gpio_init(LG1_PINX, 1, 1);
    gpio_init(LG2_PINX, 1, 1);
    gpio_init(LG3_PINX, 1, 1);

    /*
     * 数据恢复，数据从从flash到plan_handle模块，再从plan_handle模块到tft模块
     */
    flash_read((uint8_t *)&plan_in, sizeof(plan_in));
    plan_inpu_to_tft();

    return;
}



/**
 * indata_to_outdata() - 将一个植物输入的计划数据转换为对应的输出数据
 * @ind: 计划输入数据的地址
 * @outd: 转换后的值得地址
 *
 * 主要的功能是根据输入的数据得到是否到达计划执行时间，以及完成计划的次数
 */
static void indata_to_outdata(plan_input *ind, plan_output *outd)
{
    // 首先得到系统时间并转换为日历时间格式
    calendar_info st = get_system_time();
    uint32_t sys_sec = calendar_to_sec(&st);

    /*
     * 因为在周期时间输入的时候没有输入年、月、秒的地方，所以将其初始化为起始值
     * 以方便计算周期时间
     */
    ind->pd_t.year = START_YEAR;
    ind->pd_t.month = 1;
    ind->pd_t.sec = 0;
    uint32_t pd_sec = calendar_to_sec(&ind->pd_t);

    /*
     * 将第一次开始的时间加上周期时长乘以已完成次数，得到当前的开始时间；
     * 结束时间的计算同开始时间。
     * 由这两个时间可以得到当前执行计划的区间。
     */
    uint32_t ctr_bg_sec = calendar_to_sec(&ind->bg_t) + pd_sec * ind->cnt;
    uint32_t ctr_ed_sec = calendar_to_sec(&ind->ed_t) + pd_sec * ind->cnt;

    // 比较系统时间是否在执行计划的区间
    if (ctr_bg_sec < sys_sec && ctr_ed_sec > sys_sec)
    {
        outd->is_reach = 1;
    }
    else
    {
        if (outd->is_reach == 1)
        {
            /*
             * 如果没有在执行区间，并且上一次在执行区间的话，表明一次计划执行完
             * 成，给计划完成次数加一。
             */
            ind->cnt++;
        }
        outd->is_reach = 0;
    }

    return;
}


/**
 * plan_ctr_exe() - 计划控制的输出执行
 * @activity: 表示执行哪一个计划活动，一个活动编号对应一个植物，当activity大于
 * 植物的编号时，表示没有计划任务需要执行。
 *
 * 主要包括IO输出控制以及云台预置位控制
 */
static void plan_ctr_exe(uint8_t activity)
{
    /*
     * isnt_first数组是为了标记每一个活动在一次计划区间内是不是第一次执行，主要
     * 是因为不能频繁的多次调用云台的预置位
     */
    static uint8_t isnt_first[OBJ_NUM] = { 0 };
    if (activity >= OBJ_NUM)
    {
        // activity大于植物的编号的话，代表没有计划任务需要执行
        for (uint8_t i = 0; i < OBJ_NUM; i++)
        {
            isnt_first[i] = 0;
        }
        gpio_set(LGRED_PINX, NO_LIGHT);
        gpio_set(LGBLUE_PINX, NO_LIGHT);
        gpio_set(LGUVB_PINX, NO_LIGHT);
        gpio_set(WATER_PINX, NO_LIGHT);
    }
    else
    {
        // 当activity对应于一个植物编号并且是第一次执行的时候，就执行计划
        if (isnt_first[activity] == 0)
        {
            orient_presetop(0, PRESET_CALL, activity + 1);
            orient_presetop(1, PRESET_CALL, activity + 1);
            gpio_set(LGRED_PINX, plan_in[activity].lg_r == 1 ? LIGHT : NO_LIGHT);
            gpio_set(LGBLUE_PINX, plan_in[activity].lg_b == 1 ? LIGHT : NO_LIGHT);
            gpio_set(LGUVB_PINX, plan_in[activity].lg_uvb == 1 ? LIGHT : NO_LIGHT);
            gpio_set(WATER_PINX, plan_in[activity].water == 1 ? LIGHT : NO_LIGHT);
            isnt_first[activity] = 1;
        }

    }
    return;
}


/**
 * plan_ctr() - 计划控制
 *
 * 完成从计划数据到输出执行的功能。
 */
void plan_ctr(void)
{
    // 首先初始化活动不为任意一个植物编号
    uint8_t activity = OBJ_NUM;

    // 依次遍历每一个植物，得到需要执行的活动号，即对应的植物编号
    for (uint8_t i = 0; i < OBJ_NUM; i++)
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


/**
 * plan_handle() - 计划处理
 *
 * 完成计划模式的数据处理及输出功能，手动模式下因为简单不需要复杂的数据运算，
 * 此处处理为空，直接在其输入部分执行
 */
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

/**
 * key_func() - 完成手动和自动模式下的按键输入功能
 *
 * 在自动模式下只是数据的输入，在手动模式在包括数据的输入和对应的输出执行部分。
 */
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


/**
 * manul_ctr() - 手动控制部分
 *
 * 为空函数
 */
void manul_ctr(void)
{
    return;
}

/**
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
     * 结束时间在tft条目没有年、月、日，赋值为和开始时间相同
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


/**
 * plan_key_func() - 计划模式下按键的处理
 *
 * 主要和tft显示屏关
 */
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
#ifdef PM_DEBUG
        printf("UP_KEY 单击\n");
#endif /* PM_DEBUG */
        tft_left();
        break;
    case D_KEY:
#ifdef PM_DEBUG
        printf("UP_KEY 双击\n");
#endif /* PM_DEBUG */
        tft_up();
        break;
    case L_KEY:
#ifdef PM_DEBUG
        printf("UP_KEY 长按\n");
#endif /* PM_DEBUG */
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
#ifdef PM_DEBUG
        printf("DOWN_KEY 单击\n");
#endif /* PM_DEBUG */
        tft_right();
        break;
    case D_KEY:
#ifdef PM_DEBUG
        printf("DOWN_KEY 双击\n");
#endif /* PM_DEBUG */
        tft_down();
        break;
    case L_KEY:
#ifdef PM_DEBUG
        printf("DOWN_KEY 长按\n");
#endif /* PM_DEBUG */
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
#ifdef PM_DEBUG
        printf("OK_KEY 单击\n");
        tft_ok();
#endif /* PM_DEBUG */
        break;
    case D_KEY:
#ifdef PM_DEBUG
        printf("OK_KEY 双击\n");
#endif /* PM_DEBUG */
        break;
    case L_KEY:
#ifdef PM_DEBUG
        printf("OK_KEY 长按\n");
#endif /* PM_DEBUG */
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
#ifdef PM_DEBUG
        printf("RET_KEY 单击\n");
#endif /* PM_DEBUG */
        tft_ret();
        break;
    case D_KEY:
#ifdef PM_DEBUG
        printf("RET_KEY 双击\n");
#endif /* PM_DEBUG */
        break;
    case L_KEY:
#ifdef PM_DEBUG
        printf("RET_KEY 长按\n");
#endif /* PM_DEBUG */
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
#ifdef PM_DEBUG
        printf("MR_KEY 单击\n");
#endif /* PM_DEBUG */
        break;
    case D_KEY:
#ifdef PM_DEBUG
        printf("MR_KEY  双击\n");
#endif /* PM_DEBUG */
        break;
    case L_KEY:
#ifdef PM_DEBUG
        printf("MR_KEY 长按\n");
#endif /* PM_DEBUG */
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
#ifdef PM_DEBUG
        printf("MB_KEY 单击\n");
#endif /* PM_DEBUG */
        break;
    case D_KEY:
#ifdef PM_DEBUG
        printf("MB_KEY 双击\n");
#endif /* PM_DEBUG */
        break;
    case L_KEY:
#ifdef PM_DEBUG
        printf("MB_KEY 长按\n");
#endif /* PM_DEBUG */
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
#ifdef PM_DEBUG
        printf("MUVB_KEY 单击\n");
#endif /* PM_DEBUG */
        break;
    case D_KEY:
#ifdef PM_DEBUG
        printf("MUVB_KEY 双击\n");
#endif /* PM_DEBUG */
        break;
    case L_KEY:
#ifdef PM_DEBUG
        printf("MUVB_KEY 长按\n");
#endif /* PM_DEBUG */
        break;
    default:
        break;
    }
    return;
}


/**
 * manul_key_func() - 手动模式下的按键功能
 *
 * 包括了按键的输入和对应的输出执行部分
 */
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
#ifdef PM_DEBUG
        printf("UP_KEY 单击\n");
#endif /* PM_DEBUG */
        orient_setspeed(0, ORIENT_LEFT, 100);
        break;
    case D_KEY:
#ifdef PM_DEBUG
        printf("UP_KEY 双击\n");
#endif /* PM_DEBUG */
        orient_setspeed(0, ORIENT_RIGHT, 100);
        break;
    case L_KEY:
#ifdef PM_DEBUG
        printf("UP_KEY 长按\n");
#endif /* PM_DEBUG */
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
#ifdef PM_DEBUG
        printf("DOWN_KEY 单击\n");
#endif /* PM_DEBUG */
        orient_setspeed(1, ORIENT_LEFT, 100);
        break;
    case D_KEY:
#ifdef PM_DEBUG
        printf("DOWN_KEY 双击\n");
#endif /* PM_DEBUG */
        orient_setspeed(1, ORIENT_RIGHT, 100);
        break;
    case L_KEY:
#ifdef PM_DEBUG
        printf("DOWN_KEY 长按\n");
#endif /* PM_DEBUG */
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
#ifdef PM_DEBUG
        printf("OK_KEY 单击\n");
#endif /* PM_DEBUG */
        break;
    case D_KEY:
#ifdef PM_DEBUG
        printf("OK_KEY 双击\n");
#endif /* PM_DEBUG */
        break;
    case L_KEY:
#ifdef PM_DEBUG
        printf("OK_KEY 长按\n");
#endif /* PM_DEBUG */
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
#ifdef PM_DEBUG
        printf("RET_KEY 单击\n");
#endif /* PM_DEBUG */
        orient_setmode(0, MODE_MANUL);
        orient_setmode(1, MODE_MANUL);
        break;
    case D_KEY:
#ifdef PM_DEBUG
        printf("RET_KEY 双击\n");
#endif /* PM_DEBUG */
        break;
    case L_KEY:
#ifdef PM_DEBUG
        printf("RET_KEY 长按\n");
#endif /* PM_DEBUG */
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
#ifdef PM_DEBUG
        printf("MR_KEY 单击\n");
#endif /* PM_DEBUG */
        mlgr = !mlgr;
        gpio_set(LGRED_PINX, mlgr == 1 ? LIGHT : NO_LIGHT);
        break;
    case D_KEY:
#ifdef PM_DEBUG
        printf("MR_KEY  双击\n");
#endif /* PM_DEBUG */
        break;
    case L_KEY:
#ifdef PM_DEBUG
        printf("MR_KEY 长按\n");
#endif /* PM_DEBUG */
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
#ifdef PM_DEBUG
        printf("MB_KEY 单击\n");
#endif /* PM_DEBUG */
        mlgb = !mlgb;
        gpio_set(LGBLUE_PINX, mlgb == 1 ? LIGHT : NO_LIGHT);
        break;
    case D_KEY:
#ifdef PM_DEBUG
        printf("MB_KEY 双击\n");
#endif /* PM_DEBUG */
        break;
    case L_KEY:
#ifdef PM_DEBUG
        printf("MB_KEY 长按\n");
#endif /* PM_DEBUG */
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
#ifdef PM_DEBUG
        printf("MUVB_KEY 单击\n");
#endif /* PM_DEBUG */
        mlguvb = !mlguvb;
        gpio_set(LGUVB_PINX, mlguvb == 1 ? LIGHT : NO_LIGHT);
        break;
    case D_KEY:
#ifdef PM_DEBUG
        printf("MUVB_KEY 双击\n");
#endif /* PM_DEBUG */
        break;
    case L_KEY:
#ifdef PM_DEBUG
        printf("MUVB_KEY 长按\n");
#endif /* PM_DEBUG */
        break;
    default:
        break;
    }
    return;
}
