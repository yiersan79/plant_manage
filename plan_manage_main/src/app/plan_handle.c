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

#include "include/plan_handle.h"
#include "include/pm_time.h"
#include "include/tft.h"
#include "config.h"

typedef struct plan_output_
{
    uint8_t is_lgreach;
    uint8_t is_wtreach;
    uint8_t lg_cnt;
    uint8_t wt_cnt;
} plan_output;

typedef struct plan_input_
{
    calendar_info lgbg_t;
    calendar_info lged_t;
    calendar_info lgpd_t;

    calendar_info wtbg_t;
    calendar_info wted_t;
    calendar_info wtpd_t;

    uint8_t x_orient;
    uint8_t y_orient;
    uint8_t lg_r : 1;
    uint8_t lg_b : 1;
    uint8_t lg_uvb : 1;
    uint8_t water : 1;
    uint8_t sw : 1;
} plan_input;

static void indata_to_outdata(plan_input *ind, plan_output *outd);
static void ctr_exe(uint8_t activity);


static plan_output plan_out[PLAN_DATA_NUM] = { 0 };
plan_input plan_in[PLAN_DATA_NUM] = { 0 };



void plan_handle_init(void)
{
    gpio_Interrupt_init(LGRED_PINX, GPO, GPI_DISAB);
    gpio_Interrupt_init(LGBLUE_PINX, GPO, GPI_DISAB);
    gpio_Interrupt_init(LGUVB_PINX, GPO, GPI_DISAB);
    gpio_Interrupt_init(WATER_PINX, GPO, GPI_DISAB);
    // 还有设置方向的初始化
    return;
}



static void indata_to_outdata(plan_input *ind, plan_output *outd)
{
    calendar_info st = get_system_time();
    uint32_t sys_sec = calendar_to_sec(&st);

    uint32_t lgpd_sec = calendar_to_sec(&ind->lgpd_t);
    uint32_t crt_lgbg_sec = calendar_to_sec(&ind->lgbg_t) + lgpd_sec * outd->lg_cnt;
    uint32_t crt_lged_sec = calendar_to_sec(&ind->lged_t) + lgpd_sec * outd->lg_cnt;

    uint32_t wtpd_sec = calendar_to_sec(&ind->wtpd_t);
    uint32_t crt_wtbg_sec = calendar_to_sec(&ind->wtbg_t) + wtpd_sec * outd->wt_cnt;
    uint32_t crt_wted_sec = calendar_to_sec(&ind->wted_t) + wtpd_sec * outd->wt_cnt;

    if (crt_lgbg_sec < sys_sec && crt_lged_sec > sys_sec)
    {
        outd->is_lgreach = 1;
    }
    else
    {
        if (outd->is_lgreach == 1)
        {
            outd->lg_cnt++;
        }
        outd->is_lgreach = 0;
    }

    if (crt_wtbg_sec < sys_sec && crt_wted_sec > sys_sec)
    {
        outd->is_wtreach = 1;
    }
    else
    {
        if (outd->is_wtreach== 1)
        {
            outd->wt_cnt++;
        }
        outd->is_wtreach = 0;
    }
    return;
}

static void ctr_exe(uint8_t activity)
{
    if (activity >= PLAN_DATA_NUM)
    {
        gpio_set(LGRED_PINX, 0);
        gpio_set(LGBLUE_PINX, 0);
        gpio_set(LGUVB_PINX, 0);
        gpio_set(WATER_PINX, 0);
    }
    else
    {
        //set_orient(plan_in[activity].x_orient, plan_in[activity].y_orient);
        gpio_set(LGRED_PINX, plan_in[activity].lg_r);
        gpio_set(LGBLUE_PINX, plan_in[activity].lg_b);
        gpio_set(LGUVB_PINX, plan_in[activity].lg_uvb);
        gpio_set(WATER_PINX, plan_in[activity].water);
    }
    return;
}


void plan_handle(void)
{
    uint8_t activity = PLAN_DATA_NUM;

    for (uint8_t i = 0; i < PLAN_DATA_NUM; i++)
    {
        indata_to_outdata(&plan_in[i], &plan_out[i]);
        if (plan_in[i].sw == 1)
        {
            if (plan_out[i].is_lgreach == 1 || plan_out[i].is_wtreach == 1)
            {
                activity = i;
                break;
            }
        }
    }
    ctr_exe(activity);
    return;
}

