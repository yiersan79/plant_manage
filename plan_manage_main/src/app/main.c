
#include <stdio.h>
#include <stdlib.h>

#include "common.h"

#include "include/pm_init.h"
#include "include/simulat_timer.h"
#include "include/key.h"
#include "include/knob.h"
#include "include/tft.h"
#include "include/config.h"
#include "include/pm_time.h"
#include "include/plan_handle.h"


void main(void)
{
    
    pm_init();

    st_init(0, COMPARE, 10);    // 作为按键扫描函数的定时使用，在simulat_timer.c的底层中使用
    st_init(1, COMPARE, 500);

    st_init(2, COMPARE, 1);
    st_init(3, COMPARE, 500);

    calendar_info sys_cal = {
        0, 1, 2, 3, 4, 2012, 0, 1
    };
    ds1302_set_time(&sys_cal);

    knob_enable();
    
    while (1)
    {
        /*
         * 时间以及计划处理
         */
        if (st_tcf(1) == 1)
        {
            maintain_system_time();

            calendar_info st= get_system_time();
            *get_value_of_kvp("st_y", 0) = st.year;
            *get_value_of_kvp("st_mo", 0) = st.month;
            *get_value_of_kvp("st_d", 0) = st.mday;
            *get_value_of_kvp("st_h", 0) = st.hour;
            *get_value_of_kvp("st_mi", 0) = st.min;
            *get_value_of_kvp("st_s", 0) = st.sec;
            tft_page_refresh();

            plan_handle();
        }

        /*
         * 调试相关，较短时间响应
         */
        if (st_tcf(2) == 1)
        {
            static int32_t log = 0, knob_v;
            if (log != (knob_v = get_knob_val()))
            {
                printf("knob = %d\n", knob_v);
                log = knob_v;
            }
        }

        /*
         * 调试相关，较长时间响应
         */
        if (st_tcf(3) == 1)
        {
            calendar_info st = get_system_time();
            printf("%d 年 %d 月 %d 星期 %d 日 %d 时 %d 分 %d 秒\n", 
                    st.year, st.month, st.wday, st.mday, 
                    st.hour, st.min, st.sec);
            printf("sec = %d\n", calendar_to_sec(&st));
        }
        
        /*
         * 按键功能注册部分
         */
        switch (get_key_mean(UP_KEY))
        {
        case N_KEY:
            //printf("无键\n");
            break;
        case S_KEY:
            printf("单击\n");
            tft_left();
            break;
        case D_KEY:
            printf("双击\n");
            tft_up();
            break;
        case L_KEY:
            printf("长按\n");
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
            printf("单击\n");
            tft_right();
            break;
        case D_KEY:
            printf("双击\n");
            tft_down();
            break;
        case L_KEY:
            printf("长按\n");
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
            printf("单击\n");
            tft_ok();
            break;
        case D_KEY:
            printf("双击\n");
            break;
        case L_KEY:
            printf("长按\n");
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
            printf("单击\n");
            tft_ret();
            break;
        case D_KEY:
            printf("双击\n");
            break;
        case L_KEY:
            printf("长按\n");
            break;
        default:
            break;
        }
    }
    return;
}
