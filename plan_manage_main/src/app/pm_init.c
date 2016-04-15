/*
 * pm_init.c - 对整个应用程序的初始化
 */

#include "include/simulat_timer.h"
#include "include/key.h"
#include "include/knob.h"
#include "include/debug.h"


/*
 * pm_init() - 初始化
 */
void pm_init(void)
{
    debug_init();
    key_init();
    knob_init();
    st_base_init();
}
