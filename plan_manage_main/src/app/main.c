
#include <stdio.h>
#include <stdlib.h>

#include "include/pm_init.h"
#include "include/simulat_timer.h"
#include "include/key.h"
#include "include/knob.h"


void main(void)
{
    int32_t log = 0, knob_v;
    
    pm_init();

    st_init(0, COMPARE, 10);
    st_init(1, COMPARE, 1);

    knob_enable();

    while (1)
    {
        if (st_tcf(0) == 1)
        {
            key_detect();
        }
        if (st_tcf(1) == 1)
        {
            if (log != (knob_v = get_knob_val()))
            {
                printf("knob = %d\n", knob_v);
                log = knob_v;
            }
        }
        
        switch (get_key_mean(OK_KEY))
        {
        case N_KEY:
            //printf("无键\n");
            break;
        case S_KEY:
            printf("单击\n");
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
