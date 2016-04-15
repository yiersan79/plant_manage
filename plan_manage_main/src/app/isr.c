/*
 * isr.c - 中断服务函数
 */

#include <stdio.h>

#include "include.h"

#include "include/isr.h"
#include "include/simulat_timer.h"
#include "include/debug.h"
#include "include/config.h"
#include "include/knob.h"



/*
 * PIT_isr() - pit模块的中断服务函数
 */
void PIT_isr(void)
{
    if ((PIT_TFLG(SIMULAT_TIMER_PITX) & PIT_TFLG_TIF_MASK) != 0)
    {
        PIT_Flag_Clear(SIMULAT_TIMER_PITX);
        st_base();
    }
    return;
}


/*
 * PTA_isr() - PTA中断服务函数
 */
void PTA_isr(void)
{
    PORT_FUNC(A, KNOB_A_PINX, knob_detect);
    
    return;
}

/*
 * PTD_isr()
 */
void PTD_isr()
{
    PORT_FUNC(D, KNOB_A_PINX, knob_detect);
    return;
}

/*
 * UART0_isr() - uart0中断服务函数
 */
void UART0_isr(void)
{
    rec_debug_uart();
    return;
}
