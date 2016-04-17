
#include <stdio.h>
#include <stdlib.h>

#include "include/pm_init.h"
#include "include/simulat_timer.h"
#include "include/key.h"
#include "include/knob.h"
#include "include/tft.h"


void main(void)
{
    int32_t log = 0, knob_v;
    
    pm_init();

    st_init(0, COMPARE, 10);
    st_init(1, COMPARE, 1);

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
