
#include <stdio.h>
#include <stdlib.h>

#include "common.h"

#include "include/pm_init.h"
#include "include/simulat_timer.h"
#include "include/key.h"
#include "include/knob.h"
#include "include/tft.h"
#include "include/config.h"
#include "include/time.h"


void main(void)
{
    int32_t log = 0, knob_v;
    time_info time = {
        2016, 4, 1, 18, 14, 9, 0
    };
    
    pm_init();

    st_init(0, COMPARE, 10);    // 作为按键扫描函数的定时使用，在simulat_timer.c的底层中使用
    st_init(1, COMPARE, 1);

    st_init(2, COMPARE, 500);

    st_init(3, COMPARE, 500);

    ds1302_set_time(time);

    knob_enable();

    while (1)
    {

        if (st_tcf(1) == 1)
        {
            if (log != (knob_v = get_knob_val()))
            {
                printf("knob = %d\n", knob_v);
                log = knob_v;
            }
        }
        if (st_tcf(2) == 1)
        {
            enter_critical();
            ds1302_read_time(&time);
            exit_critical();
            printf("%d 年 %d 月 %d 星期 %d 日 %d 时 %d 分 %d 秒\n", 
                    time.year,time.month, time.week, time.day, 
                    time.hour, time.minute, time.sec);
        }
        
        if (st_tcf(3) == 1)
        {
            enter_critical();
            ds1302_read_time(&time);
            exit_critical();
            *get_value_of_kvp("st_y", 0) = time.year;
            *get_value_of_kvp("st_mo", 0) = time.month;
            *get_value_of_kvp("st_d", 0) = time.day;
            *get_value_of_kvp("st_h", 0) = time.hour;
            *get_value_of_kvp("st_mi", 0) = time.minute;
            *get_value_of_kvp("st_s", 0) = time.sec;
            tft_page_refresh();
        }
        
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
