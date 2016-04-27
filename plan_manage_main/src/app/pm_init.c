/*
 * pm_init.c - 对整个应用程序的初始化
 */

#include "include/simulat_timer.h"
#include "include/key.h"
#include "include/knob.h"
#include "include/debug.h"
#include "include/tft.h"
#include "include/config.h"
#include "include/pm_time.h"
#include "include/plan_handle.h"
#include "include/pm_flash.h"
#include "include/orient.h"


/*
 * pm_init() - 初始化
 */
void pm_init(void)
{
    pm_flash_init();
    debug_init();
    key_init();
    knob_init();
    st_base_init();
    tft_init();
    ds1302_init();
    plan_handle_init();     // 计划处理初始化必须在flash初始化之后
    orient_init();
    return;
}
