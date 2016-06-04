/*
 * tft.c - tft 显示屏的接口
 *
 * uart hmi方式
 * 这个模块的核心是一个状态机框架，如下
 *    switch (tft_stt.pgn)
 *    {
 *    case ORIGINAL_PG:
 *        switch (kvp_menu[etn].attr)
 *        {
 *        case R_NUM:
 *            break;
 *        case RW_NUM:
 *            break;
 *        case RW_PIC:
 *            break;
 *        case SW_PAGE:
 *            break;
 *        case R_TXT:
 *            break;
 *        default:
 *            break;
 *        }
 *        break;
 *    case MENU_PG:
 *        switch (kvp_menu[etn].attr)
 *        {
 *        case R_NUM:
 *            break;
 *        case RW_NUM:
 *            break;
 *        case RW_PIC:
 *            break;
 *        case SW_PAGE:
 *            break;
 *        case R_TXT:
 *            break;
 *        default:
 *            break;
 *        }
 *        break;
 *    case OBJ_SET_PG:
 *        switch (kvp_obj_set[tft_stt.objn][etn].attr)
 *        {
 *        case R_NUM:
 *            break;
 *        case RW_NUM:
 *            break;
 *        case RW_PIC:
 *            break;
 *        case SW_PAGE:
 *            break;
 *        case R_TXT:
 *            break;
 *        default:
 *            break;
 *        }
 *        break;
 *    default:
 *        break;
 *    }
 * 关于显示的所有，最终都会体现在这个框架里，也就是所谓的状态机里，逻辑简单，细节繁琐
 *
 * 解释几个关键变量：
 *      tft_stt - 包含状态机当前的状态信息
 *      kvp_* - 数组，每一个元素具有一个页面中单个元素的数据信息，它的集合组成整个页
 *      面的数据信息，它的元素顺序应该符合页面中的实际顺序，也即它的元素顺序表示了页
 *      面元素的顺序信息
 *      *_lyt - 数组，配合kvp_*描述一个页面的布局，其每一个元素表示页面中对应行中第一
 *      个元素在kvp_*数组中的下标
 *      每一对*_lyt和kvp_*完整的表示了一个页面，也就是数据信息和布局信息
 *      ( * 号意指通配符；页面是按行分割的)
 *
 * 所有如上，所有如下
 */

#include <stdint.h>
#include <string.h>
#include <stdio.h>

#include "uart.h"

#include "include/tft.h"
#include "include/knob.h"
#include "include/key.h"
#include "include/config.h"
#include "include/plan_handle.h"
#include "include/tft_plan_internal.h"
#include "include/pm_flash.h"
#include "include/orient.h"






/*
 * tft_state - 在tft操作中，表示当前状态机的状态的类型
 * pgn表示页面编号，etn、ln分别一个页面中的元素号、行号，objn表示对象号。
 */
typedef struct tft_state_
{
    uint8_t pgn;
    uint8_t etn;
    uint8_t ln;
    uint8_t objn;
} tft_state;

/*
 * 页面标志
 * ORIGINAL_PG表示开机界面，MENU_PG表示主界面，OBJ_SET_PG表示植物属性设置界面
 */
typedef enum page_name_
{
    ORIGINAL_PG = 0, MENU_PG, OBJ_SET_PG
} page_name;

/*
 * 页面属性标志
 * 枚举的每个成员的含义同其标识符
 */
typedef enum entry_attr_
{
    R_NUM = 0, RW_NUM, RW_PIC, SW_PAGE, R_TXT, RW_TXT
} entry_attr;

/*
 * tft设置颜色时需要的值
 */
typedef enum tft_colour_
{
    TFT_BACK = 65535, TFT_RED = 63488, TFT_PURPLE = 31
} tft_colour;





// tft显示屏当前状态变量定义
static tft_state tft_stt = { 0, 0, 0, 0 };

/*
 * original_lyt数组存储的是开机界面的每一行第一个有效元素的元素编号
 * menu_lyt数组存储的是主界面的每一行第一个有效元素的元素编号
 * obj_set_lyt数组存储的是植物属性设置界面的每一行第一个有效元素的元素编号
 */
static uint8_t original_lyt[] = { 0, 1 };
static uint8_t menu_lyt[] = { 0, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15 };
static uint8_t obj_set_lyt[] = { 0, 2, 5, 9, 13, 16, 18, 19};


/*
 * kvp_original数组中每一个元素按顺序依次对应着开机界面中的每一个有效元素。
 * kvp_menu数组对应主界面。
 * kvp_obj_set对应八个植物属性界面，这个数组中有八个相同形式的数组，对应八个不
 * 同的植物。
 *
 * 需要说明的地方有两个，一是数组中的每一个元素都是有序的，是和显示页面中的有效
 * 元素顺序对应的；二是数组中的每一个元素包含的信息有一个字符串，一个整型值，
 * 一个枚举值，字符串对应着页面元素的ID，整型值在不同的情况下有不同的含义，枚举
 * 值表示了元素的类型。
 */
static kv_pair kvp_original[] = { {"ori_0", 0, R_NUM} };
static kv_pair kvp_menu[] = {
    { "st_y", 2016, RW_NUM }, { "st_mo", 4, RW_NUM }, { "st_d", 14, RW_NUM }, { "st_h", 0, RW_NUM }, { "st_mi", 0, RW_NUM }, { "st_s", 0, RW_NUM },
    { "obj0", 0, SW_PAGE },
    { "obj1", 1, SW_PAGE },
    { "obj2", 2, SW_PAGE },
    { "obj3", 3, SW_PAGE },
    { "obj4", 4, SW_PAGE },
    { "obj5", 5, SW_PAGE },
    { "obj6", 6, SW_PAGE },
    { "obj7", 7, SW_PAGE },
    { "note", 0, R_TXT }
};
kv_pair kvp_obj_set[][PLAN_DATA_NUM] =
{
    {
        { "ob", 1, R_NUM}, { "sw", 1, RW_PIC },// 0~1
        { "bg_y", 2016, RW_NUM }, { "bg_mo", 4, RW_NUM }, { "bg_d", 14, RW_NUM }, // 2~4
        { "bg_h", 14, RW_NUM }, { "bg_mi", 0, RW_NUM }, { "ed_h", 14, RW_NUM }, { "ed_mi", 0, RW_NUM }, // 5~8
        { "lg_r", 0, RW_PIC}, { "lg_b", 0, RW_PIC }, { "lg_uvb", 0, RW_PIC }, { "water", 0, RW_PIC }, // 9~12
        { "pd_d", 3, RW_NUM }, { "pd_h", 3, RW_NUM }, { "pd_mi", 3, RW_NUM }, // 13~15
        { "x", 0, RW_TXT }, { "y", 1, RW_TXT }, // 16~17
        { "cnt", 0, RW_NUM } //18
    },  // 0
    {
        { "ob", 2, R_NUM}, { "sw", 1, RW_PIC },// 0~1
        { "bg_y", 2016, RW_NUM }, { "bg_mo", 4, RW_NUM }, { "bg_d", 14, RW_NUM }, // 2~4
        { "bg_h", 14, RW_NUM }, { "bg_mi", 0, RW_NUM }, { "ed_h", 14, RW_NUM }, { "ed_mi", 0, RW_NUM }, // 5~8
        { "lg_r", 0, RW_PIC}, { "lg_b", 0, RW_PIC }, { "lg_uvb", 0, RW_PIC }, { "water", 0, RW_PIC }, // 9~12
        { "pd_d", 3, RW_NUM }, { "pd_h", 3, RW_NUM }, { "pd_mi", 3, RW_NUM }, // 13~15
        { "x", 0, RW_TXT }, { "y", 1, RW_TXT }, // 16~17
        { "cnt", 0, RW_NUM } //18
    }, // 1
    {
        { "ob", 3, R_NUM}, { "sw", 1, RW_PIC },// 0~1
        { "bg_y", 2016, RW_NUM }, { "bg_mo", 4, RW_NUM }, { "bg_d", 14, RW_NUM }, // 2~4
        { "bg_h", 14, RW_NUM }, { "bg_mi", 0, RW_NUM }, { "ed_h", 14, RW_NUM }, { "ed_mi", 0, RW_NUM }, // 5~8
        { "lg_r", 0, RW_PIC}, { "lg_b", 0, RW_PIC }, { "lg_uvb", 0, RW_PIC }, { "water", 0, RW_PIC }, // 9~12
        { "pd_d", 3, RW_NUM }, { "pd_h", 3, RW_NUM }, { "pd_mi", 3, RW_NUM }, // 13~15
        { "x", 0, RW_TXT }, { "y", 1, RW_TXT }, // 16~17
        { "cnt", 0, RW_NUM } //18
    }, // 2
    {
        { "ob", 4, R_NUM}, { "sw", 1, RW_PIC },// 0~1
        { "bg_y", 2016, RW_NUM }, { "bg_mo", 4, RW_NUM }, { "bg_d", 14, RW_NUM }, // 2~4
        { "bg_h", 14, RW_NUM }, { "bg_mi", 0, RW_NUM }, { "ed_h", 14, RW_NUM }, { "ed_mi", 0, RW_NUM }, // 5~8
        { "lg_r", 0, RW_PIC}, { "lg_b", 0, RW_PIC }, { "lg_uvb", 0, RW_PIC }, { "water", 0, RW_PIC }, // 9~12
        { "pd_d", 3, RW_NUM }, { "pd_h", 3, RW_NUM }, { "pd_mi", 3, RW_NUM }, // 13~15
        { "x", 0, RW_TXT }, { "y", 1, RW_TXT }, // 16~17
        { "cnt", 0, RW_NUM } //18
    }, // 3
    {
        { "ob", 5, R_NUM}, { "sw", 1, RW_PIC },// 0~1
        { "bg_y", 2016, RW_NUM }, { "bg_mo", 4, RW_NUM }, { "bg_d", 14, RW_NUM }, // 2~4
        { "bg_h", 14, RW_NUM }, { "bg_mi", 0, RW_NUM }, { "ed_h", 14, RW_NUM }, { "ed_mi", 0, RW_NUM }, // 5~8
        { "lg_r", 0, RW_PIC}, { "lg_b", 0, RW_PIC }, { "lg_uvb", 0, RW_PIC }, { "water", 0, RW_PIC }, // 9~12
        { "pd_d", 3, RW_NUM }, { "pd_h", 3, RW_NUM }, { "pd_mi", 3, RW_NUM }, // 13~15
        { "x", 0, RW_TXT }, { "y", 1, RW_TXT }, // 16~17
        { "cnt", 0, RW_NUM } //18
    }, // 4
    {
        { "ob", 6, R_NUM}, { "sw", 1, RW_PIC },// 0~1
        { "bg_y", 2016, RW_NUM }, { "bg_mo", 4, RW_NUM }, { "bg_d", 14, RW_NUM }, // 2~4
        { "bg_h", 14, RW_NUM }, { "bg_mi", 0, RW_NUM }, { "ed_h", 14, RW_NUM }, { "ed_mi", 0, RW_NUM }, // 5~8
        { "lg_r", 0, RW_PIC}, { "lg_b", 0, RW_PIC }, { "lg_uvb", 0, RW_PIC }, { "water", 0, RW_PIC }, // 9~12
        { "pd_d", 3, RW_NUM }, { "pd_h", 3, RW_NUM }, { "pd_mi", 3, RW_NUM }, // 13~15
        { "x", 0, RW_TXT }, { "y", 1, RW_TXT }, // 16~17
        { "cnt", 0, RW_NUM } //18
    }, // 5
    {
        { "ob", 7, R_NUM}, { "sw", 1, RW_PIC },// 0~1
        { "bg_y", 2016, RW_NUM }, { "bg_mo", 4, RW_NUM }, { "bg_d", 14, RW_NUM }, // 2~4
        { "bg_h", 14, RW_NUM }, { "bg_mi", 0, RW_NUM }, { "ed_h", 14, RW_NUM }, { "ed_mi", 0, RW_NUM }, // 5~8
        { "lg_r", 0, RW_PIC}, { "lg_b", 0, RW_PIC }, { "lg_uvb", 0, RW_PIC }, { "water", 0, RW_PIC }, // 9~12
        { "pd_d", 3, RW_NUM }, { "pd_h", 3, RW_NUM }, { "pd_mi", 3, RW_NUM }, // 13~15
        { "x", 0, RW_TXT }, { "y", 1, RW_TXT }, // 16~17
        { "cnt", 0, RW_NUM } //18
    }, // 6
    {
        { "ob", 8, R_NUM}, { "sw", 1, RW_PIC },// 0~1
        { "bg_y", 2016, RW_NUM }, { "bg_mo", 4, RW_NUM }, { "bg_d", 14, RW_NUM }, // 2~4
        { "bg_h", 14, RW_NUM }, { "bg_mi", 0, RW_NUM }, { "ed_h", 14, RW_NUM }, { "ed_mi", 0, RW_NUM }, // 5~8
        { "lg_r", 0, RW_PIC}, { "lg_b", 0, RW_PIC }, { "lg_uvb", 0, RW_PIC }, { "water", 0, RW_PIC }, // 9~12
        { "pd_d", 3, RW_NUM }, { "pd_h", 3, RW_NUM }, { "pd_mi", 3, RW_NUM }, // 13~15
        { "x", 0, RW_TXT }, { "y", 1, RW_TXT }, // 16~17
        { "cnt", 0, RW_NUM } //18
    }, // 7
};

static char tft_cmd_str[20];

/*
 * 静态函数声明
 */
static void tft_send_cmd(const char *cmd);
static void tft_set_color(uint8_t etn, tft_colour tft_col);
static void tft_input(void);
static void sw_to_obj(void);




/**
 * tft_init() - tft模块初始化
 *
 * 包括串口初始化和初始化显示屏为开机界面
 */
void tft_init(void)
{
    uart_init(TFT_UARTX, 9600);
    tft_send_cmd("page original");
    return;
}

/**
 * tft_send_cmd() - 向串口屏发送命令
 */
static void tft_send_cmd(const char *cmd)
{
    uart_sendStr(TFT_UARTX, (const uint8_t *)cmd);

    uart_putchar(TFT_UARTX, 0xff);
    uart_putchar(TFT_UARTX, 0xff);
    uart_putchar(TFT_UARTX, 0xff);
}

/**
 * tft_set_color() - 设置tft显示屏页面中一个元素区域的颜色
 * @etn: 要设置颜色的区域对应的元素号
 * @tft_col: 要设置的颜色
 *
 * 设置颜色时，首先得到对应的元素ID，接着产生一个对此ID设置颜色属性的字符串形式
 * 的命令，然后通过串口将这个字符串发送给串口屏，最后还要发送刷新此ID的命令以使
 * 颜色变化显示出来
 */
static void tft_set_color(uint8_t etn, tft_colour tft_col)
{
    //对于不同的页面有不同的处理
    switch (tft_stt.pgn)
    {
    case ORIGINAL_PG:
        break;
    case MENU_PG:
        // 对同一页面中不同属性的元素做不同的处理
        switch (kvp_menu[etn].attr)
        {
        case R_NUM:
        case RW_NUM:
        case R_TXT:
        case RW_TXT:
            // 更改此元素的颜色属性
            sprintf(tft_cmd_str, "%s.bco=%u", kvp_menu[etn].key, tft_col);
            tft_send_cmd(tft_cmd_str);
            // 刷新此元素
            sprintf(tft_cmd_str, "ref %s", kvp_menu[etn].key);
            tft_send_cmd(tft_cmd_str);
            break;
        case RW_PIC:
        case SW_PAGE:
            sprintf(tft_cmd_str, "%s_sg.bco=%u", kvp_menu[etn].key, tft_col);
            tft_send_cmd(tft_cmd_str);
            sprintf(tft_cmd_str, "ref %s_sg", kvp_menu[etn].key);
            tft_send_cmd(tft_cmd_str);
            break;
        default:
            break;
        }
        break;
    case OBJ_SET_PG:
        switch (kvp_obj_set[tft_stt.objn][etn].attr)
        {
        case R_NUM:
        case RW_NUM:
        case R_TXT:
        case RW_TXT:
            sprintf(tft_cmd_str, "%s.bco=%u",
                    kvp_obj_set[tft_stt.objn][etn].key, tft_col);
            tft_send_cmd(tft_cmd_str);
            sprintf(tft_cmd_str, "ref %s", kvp_obj_set[tft_stt.objn][etn].key);
            tft_send_cmd(tft_cmd_str);
            break;
        case RW_PIC:
        case SW_PAGE:
            sprintf(tft_cmd_str, "%s_sg.bco=%u",
                    kvp_obj_set[tft_stt.objn][etn].key, tft_col);
            tft_send_cmd(tft_cmd_str);
            sprintf(tft_cmd_str, "ref %s_sg",
                    kvp_obj_set[tft_stt.objn][etn].key);
            tft_send_cmd(tft_cmd_str);
            break;
        default:
            break;
        }
        break;
    default:
        break;
    }
    return;
}

/**
 * tft_up() - 选择tft显示屏当前页面当前行的上一行的第一个元素
 *
 * 当前被选中的元素将以显著的颜色标示出来
 */
void tft_up(void)
{
    // 记录当前元素号
    uint8_t etn_log = tft_stt.etn;

    // 对同一页面做不同的处理
    switch (tft_stt.pgn)
    {
    case ORIGINAL_PG:
        // 当前行号大于零就减一表示上一行
        if (tft_stt.ln > 0)
        {
            tft_stt.ln--;
        }
        // 设置元素号为此时行号对应行的第一个元素的元素号
        tft_stt.etn  = original_lyt[tft_stt.ln];

        // 取消之前的元素的颜色标记，并标记当前元素
        tft_set_color(etn_log, TFT_BACK);
        tft_set_color(tft_stt.etn, TFT_PURPLE);
        break;
    case MENU_PG:
        if (tft_stt.ln > 0)
        {
            tft_stt.ln--;
        }
        tft_stt.etn  = menu_lyt[tft_stt.ln];

        tft_set_color(etn_log, TFT_BACK);
        tft_set_color(tft_stt.etn, TFT_PURPLE);
        break;
    case OBJ_SET_PG:
        if (tft_stt.ln > 0)
        {
            tft_stt.ln--;
        }
        tft_stt.etn  = obj_set_lyt[tft_stt.ln];

        tft_set_color(etn_log, TFT_BACK);
        tft_set_color(tft_stt.etn, TFT_PURPLE);
        break;
    default:
        break;
    }
    return;
}

/**
 * tft_down() - 选择tft显示屏当前页面当前行的下一行的第一个元素
 *
 * 当前被选中的元素将以显著的颜色标示出来
 */
void tft_down(void)
{
    uint8_t etn_log = tft_stt.etn;

    switch (tft_stt.pgn)
    {
    case ORIGINAL_PG:
        // 如果当前行号小于最大行号的话，当前行号加一
        if (tft_stt.ln < sizeof(original_lyt) / sizeof(uint8_t) - 2)
        {
            tft_stt.ln++;
        }
        tft_stt.etn  = original_lyt[tft_stt.ln];

        tft_set_color(etn_log, TFT_BACK);
        tft_set_color(tft_stt.etn, TFT_PURPLE);
        break;
    case MENU_PG:
        if (tft_stt.ln < sizeof(menu_lyt) / sizeof(uint8_t) - 2)
        {
            tft_stt.ln++;
        }
        tft_stt.etn  = menu_lyt[tft_stt.ln];

        tft_set_color(etn_log, TFT_BACK);
        tft_set_color(tft_stt.etn, TFT_PURPLE);
        break;
    case OBJ_SET_PG:
        if (tft_stt.ln < sizeof(obj_set_lyt) / sizeof(uint8_t) - 2)
        {
            tft_stt.ln++;
        }
        tft_stt.etn  = obj_set_lyt[tft_stt.ln];

        tft_set_color(etn_log, TFT_BACK);
        tft_set_color(tft_stt.etn, TFT_PURPLE);
        break;
    default:
        break;
    }
    return;
}

/**
 * tft_left() - 选择tft显示屏当前页面中当前元素的下一个元素
 *
 * 当前被选中的元素将以显著的颜色标示出来
 */
void tft_left(void)
{
    uint8_t etn_log = tft_stt.etn;

    switch (tft_stt.pgn)
    {
    case ORIGINAL_PG:
        if (tft_stt.etn > 0)
        {
            if (--tft_stt.etn < original_lyt[tft_stt.ln])
            {
                tft_stt.ln--;
            }
        }

        tft_set_color(etn_log, TFT_BACK);
        tft_set_color(tft_stt.etn, TFT_PURPLE);
        break;
    case MENU_PG:
        if (tft_stt.etn > 0)
        {
            if (--tft_stt.etn < menu_lyt[tft_stt.ln])
            {
                tft_stt.ln--;
            }
        }

        tft_set_color(etn_log, TFT_BACK);
        tft_set_color(tft_stt.etn, TFT_PURPLE);
        break;
    case OBJ_SET_PG:
        if (tft_stt.etn > 0)
        {
            if (--tft_stt.etn < obj_set_lyt[tft_stt.ln])
            {
                tft_stt.ln--;
            }
        }

        tft_set_color(etn_log, TFT_BACK);
        tft_set_color(tft_stt.etn, TFT_PURPLE);
        break;
    default:
        break;
    }
    return;
}

/**
 * tft_left() - 选择tft显示屏当前页面中当前元素的上一个元素
 *
 * 当前被选中的元素将以显著的颜色标示出来
 */
void tft_right(void)
{
    uint8_t etn_log = tft_stt.etn;

    switch (tft_stt.pgn)
    {
    case ORIGINAL_PG:
        if (tft_stt.etn < original_lyt[sizeof(original_lyt)
                / sizeof(uint8_t) - 1] - 1)
        {
            if (++tft_stt.etn >= original_lyt[tft_stt.ln + 1])
            {
                tft_stt.ln++;
            }
        }

        tft_set_color(etn_log, TFT_BACK);
        tft_set_color(tft_stt.etn, TFT_PURPLE);
        break;
    case MENU_PG:
        if (tft_stt.etn < menu_lyt[sizeof(menu_lyt) / sizeof(uint8_t) - 1] - 1)
        {
            if (++tft_stt.etn >= menu_lyt[tft_stt.ln + 1])
            {
                tft_stt.ln++;
            }
        }

        tft_set_color(etn_log, TFT_BACK);
        tft_set_color(tft_stt.etn, TFT_PURPLE);
        break;
    case OBJ_SET_PG:
        if (tft_stt.etn < obj_set_lyt[sizeof(obj_set_lyt)
                / sizeof(uint8_t) - 1] - 1)
        {
            if (++tft_stt.etn >= obj_set_lyt[tft_stt.ln + 1])
            {
                tft_stt.ln++;
            }
        }

        tft_set_color(etn_log, TFT_BACK);
        tft_set_color(tft_stt.etn, TFT_PURPLE);
        break;
    default:
        break;
    }
    return;
}



/**
 * tft_ret() - 串口屏返回上一个页面
 */
void tft_ret(void)
{
    // 对于不同的页面做不同的处理
    switch (tft_stt.pgn)
    {
    case ORIGINAL_PG:
        break;
    case MENU_PG:
        /*
         * 更新状态
         * 发送显示上一个页面的命令，
         * 设置此时页面选中标记为当前页面中的第一个元素
         */
        tft_stt.pgn = ORIGINAL_PG;
        tft_stt.ln = 0;
        tft_stt.etn = 0;
        tft_send_cmd("page original");
        tft_page_refresh();
        tft_set_color(tft_stt.etn, TFT_PURPLE);
        break;
    case OBJ_SET_PG:
        tft_stt.pgn = MENU_PG;
        tft_stt.ln = 0;
        tft_stt.etn = 0;
        tft_send_cmd("page menu");
        tft_page_refresh();

        sw_to_obj();
        tft_to_plan_input(tft_stt.objn);
        enter_critical();
        flash_write((uint8_t *)plan_in, sizeof(plan_in));
        exit_critical();
        tft_set_color(tft_stt.etn, TFT_PURPLE);
        break;
    default:
        break;
    }
    return;
}

/**
 * tft_ok() - 串口屏进入下一个页面或者编辑状态
 */
void tft_ok(void)
{
    switch (tft_stt.pgn)
    {
    case ORIGINAL_PG:
        switch (SW_PAGE)
        {
        case R_NUM:
            break;
        case RW_NUM:
            break;
        case RW_PIC:
            break;
        case SW_PAGE:
            tft_stt.pgn = MENU_PG;
            tft_stt.ln = 0;
            tft_stt.etn = 0;
            tft_send_cmd("page menu");
            tft_page_refresh();
            sw_to_obj();
            tft_set_color(tft_stt.etn, TFT_PURPLE);
            break;
        default:
            break;
        }
        break;
    case MENU_PG:
        switch (kvp_menu[tft_stt.etn].attr)
        {
        case R_NUM:
            break;
        // 进行RW_NUM属性元素的编辑
        case RW_NUM:
            tft_input();
            break;
        // 进行RW_TXT属性元素的编辑
        case RW_TXT:
            tft_input();
            break;
        // 进行RW_PIC属性元素的编辑
        case RW_PIC:
            tft_input();
            break;
        // 进入下一个页面
        case SW_PAGE:
            tft_stt.pgn = OBJ_SET_PG;
            tft_stt.objn = kvp_menu[tft_stt.etn].value;
            tft_stt.ln = 0;
            tft_stt.etn = 0;
            tft_send_cmd("page obj_set");
            tft_page_refresh();
            tft_set_color(tft_stt.etn, TFT_PURPLE);
            break;
        default:
            break;
        }
        break;
    case OBJ_SET_PG:
        switch (kvp_obj_set[tft_stt.objn][tft_stt.etn].attr)
        {
        case R_NUM:
            break;
        case RW_TXT:
            tft_input();
            break;
        case RW_NUM:
            tft_input();
            break;
        case RW_PIC:
            tft_input();
            break;
        case SW_PAGE:
            break;
        default:
            break;
        }
        break;
    default:
        break;
    }
    return;
}

/**
 * tft_input() - 实现串口屏的编辑输入功能
 *
 * 当选中一个元素编辑时，应改变其颜色以标记进入编辑状态
 * 编辑输入时有旋钮输入和按键输入两种情况
 */
static void tft_input(void)
{
    input_limit in_lmt;
    int16_t in_v, bg_v;

    // 不同页面
    switch (tft_stt.pgn)
    {
    case ORIGINAL_PG:
        break;
    case MENU_PG:
        // 不同属性
        switch (kvp_menu[tft_stt.etn].attr)
        {
        case R_NUM:
            break;
        // 旋钮输入
        case RW_NUM:
            // 设置颜色
            tft_set_color(tft_stt.etn, TFT_RED);

            // 得到当前编辑的元素输入区间范围
            in_lmt = tft_input_limit(kvp_menu[tft_stt.etn].key);
            // 记录当前元素的初始值
            bg_v = kvp_menu[tft_stt.etn].value;
            knob_enable();
            // 在没有按RET按键的时候，一直保持在编辑状态
            while (get_key_mean(RET_KEY) == N_KEY)
            {
                in_v = bg_v + get_knob_val();
                if (in_v < in_lmt.min)
                {
                    kvp_menu[tft_stt.etn].value = in_lmt.min;
                    bg_v = in_lmt.min;
                    knob_clear();
                }
                else if (in_v > in_lmt.max)
                {
                    kvp_menu[tft_stt.etn].value = in_lmt.max;
                    bg_v = in_lmt.max;
                    knob_clear();
                }
                else
                {
                    kvp_menu[tft_stt.etn].value = in_v;
                }
                sprintf(tft_cmd_str, "%s.val=%u", kvp_menu[tft_stt.etn].key,
                        kvp_menu[tft_stt.etn].value);
                tft_send_cmd(tft_cmd_str);
            }
            knob_disable();
            tft_set_color(tft_stt.etn, TFT_PURPLE);
            // 如果是对系统时间设置的话，要同时更新系统时间
            calendar_info cal;
            cal.year = *get_value_of_kvp("st_y", 0);
            cal.month = *get_value_of_kvp("st_mo", 0);
            cal.mday = *get_value_of_kvp("st_d", 0);
            cal.hour = *get_value_of_kvp("st_h", 0);
            cal.min = *get_value_of_kvp("st_mi", 0);
            cal.sec= *get_value_of_kvp("st_s", 0);
            ds1302_set_time(&cal);
            clear_key_m();
            break;
        // 按键输入的情况
        case RW_PIC:
            tft_set_color(tft_stt.etn, TFT_RED);
            while (get_key_mean(RET_KEY) == N_KEY)
            {
                if (get_key_mean(UP_KEY) == S_KEY
                        || get_key_mean(DOWN_KEY) == S_KEY)
                {
                    kvp_menu[tft_stt.etn].value = !kvp_menu[tft_stt.etn].value;
                    sprintf(tft_cmd_str, "vis %s,%u", kvp_menu[tft_stt.etn].key,
                            kvp_menu[tft_stt.etn].value);
                    tft_send_cmd(tft_cmd_str);
                }
            }
            tft_set_color(tft_stt.etn, TFT_PURPLE);
            clear_key_m();
            break;
        case SW_PAGE:
            break;
        default:
            break;
        }
        break;
    case OBJ_SET_PG:
        switch (kvp_obj_set[tft_stt.objn][tft_stt.etn].attr)
        {
        case R_NUM:
            break;
        case RW_NUM:
            tft_set_color(tft_stt.etn, TFT_RED);
            in_lmt =
                tft_input_limit(kvp_obj_set[tft_stt.objn][tft_stt.etn].key);
            bg_v = kvp_obj_set[tft_stt.objn][tft_stt.etn].value;
            knob_enable();
            while (get_key_mean(RET_KEY) == N_KEY)
            {
                in_v = bg_v + get_knob_val();
                if (in_v < in_lmt.min)
                {
                    kvp_obj_set[tft_stt.objn][tft_stt.etn].value = in_lmt.min;
                    bg_v = in_lmt.min;
                    knob_clear();
                }
                else if (in_v > in_lmt.max)
                {
                    kvp_obj_set[tft_stt.objn][tft_stt.etn].value = in_lmt.max;
                    bg_v = in_lmt.max;
                    knob_clear();
                }
                else
                {
                    kvp_obj_set[tft_stt.objn][tft_stt.etn].value = in_v;
                }
                sprintf(tft_cmd_str, "%s.val=%u",
                        kvp_obj_set[tft_stt.objn][tft_stt.etn].key,
                        kvp_obj_set[tft_stt.objn][tft_stt.etn].value);
                tft_send_cmd(tft_cmd_str);
            }
            knob_disable();
            tft_set_color(tft_stt.etn, TFT_PURPLE);
            clear_key_m();
            break;
        case RW_PIC:
            tft_set_color(tft_stt.etn, TFT_RED);

            while (get_key_mean(RET_KEY) == N_KEY)
            {
                if (get_key_mean(UP_KEY) == S_KEY || get_key_mean(DOWN_KEY))
                {
                    kvp_obj_set[tft_stt.objn][tft_stt.etn].value =
                        !kvp_obj_set[tft_stt.objn][tft_stt.etn].value;
                    sprintf(tft_cmd_str, "vis %s,%u",
                            kvp_obj_set[tft_stt.objn][tft_stt.etn].key,
                            kvp_obj_set[tft_stt.objn][tft_stt.etn].value);
                    tft_send_cmd(tft_cmd_str);
                }
            }
            tft_set_color(tft_stt.etn, TFT_PURPLE);
            clear_key_m();
            break;
        // 设置方向
        case RW_TXT:
            tft_set_color(tft_stt.etn, TFT_RED);
            while (get_key_mean(RET_KEY) == N_KEY)
            {
                if (get_key_mean(UP_KEY) == S_KEY)
                {
                    orient_setspeed(
                            kvp_obj_set[tft_stt.objn][tft_stt.etn].value,
                            ORIENT_LEFT, 100);
                    sprintf(tft_cmd_str, "%s.txt=\"%s\"",
                            kvp_obj_set[tft_stt.objn][tft_stt.etn].key, "左转");
                    tft_send_cmd(tft_cmd_str);
                }
                else if (get_key_mean(DOWN_KEY))
                {
                    orient_setspeed(
                            kvp_obj_set[tft_stt.objn][tft_stt.etn].value,
                            ORIENT_RIGHT, 100);
                    sprintf(tft_cmd_str, "%s.txt=\"%s\"",
                            kvp_obj_set[tft_stt.objn][tft_stt.etn].key, "右转");
                    tft_send_cmd(tft_cmd_str);
                }
                else
                {
                    // 保持上一次循环
                }
            }
            orient_setmode(kvp_obj_set[tft_stt.objn][tft_stt.etn].value,
                    MODE_MANUL);
            orient_presetop(kvp_obj_set[tft_stt.objn][tft_stt.etn].value,
                    PRESET_SET, tft_stt.objn + 1);
            tft_set_color(tft_stt.etn, TFT_PURPLE);
            clear_key_m();
            break;
        case SW_PAGE:
            break;
        default:
            break;
        }
        break;
    default:
        break;
    }
}


/*
 * tft_page_refresh() - 在页面信息有改变时调用以刷新页面
 *
 * 实现的主要依据是串口屏中控件的刷新命令
 */
void tft_page_refresh(void)
{
    switch (tft_stt.pgn)
    {
    case ORIGINAL_PG:
        for (uint8_t etn = 0; etn < sizeof(kvp_original) / sizeof(kv_pair);
                etn++)
        {
            switch (SW_PAGE)
            {
            case R_NUM:
                break;
            case RW_NUM:
                break;
            case RW_PIC:
                break;
            case SW_PAGE:
                break;
            default:
                break;
            }
        }
        break;
    case MENU_PG:
        for (int etn = 0; etn < sizeof(kvp_menu) / sizeof(kv_pair);
                etn++)
        {
            switch (kvp_menu[etn].attr)
            {
            case R_NUM:
            case RW_NUM:
                sprintf(tft_cmd_str, "%s.val=%u", kvp_menu[etn].key,
                        kvp_menu[etn].value);
                tft_send_cmd(tft_cmd_str);
                sprintf(tft_cmd_str, "ref %s", kvp_menu[etn].key);
                tft_send_cmd(tft_cmd_str);
                break;
            case RW_PIC:
                sprintf(tft_cmd_str, "vis %s,%u", kvp_menu[etn].key,
                        kvp_menu[etn].value);
                tft_send_cmd(tft_cmd_str);
                break;
            case SW_PAGE:
                break;
            default:
                break;
            }
        }
        break;
    case OBJ_SET_PG:
        for (int etn = 0; etn < sizeof(kvp_obj_set[0]) / sizeof(kv_pair);
                etn++)
        {
            switch (kvp_obj_set[tft_stt.objn][etn].attr)
            {
            case R_NUM:
            case RW_NUM:
                sprintf(tft_cmd_str, "%s.val=%u",
                        kvp_obj_set[tft_stt.objn][etn].key,
                        kvp_obj_set[tft_stt.objn][etn].value);
                tft_send_cmd(tft_cmd_str);
                sprintf(tft_cmd_str, "ref %s",
                        kvp_obj_set[tft_stt.objn][etn].key);
                tft_send_cmd(tft_cmd_str);
                break;
            case RW_PIC:
                sprintf(tft_cmd_str, "vis %s,%u",
                        kvp_obj_set[tft_stt.objn][etn].key,
                        kvp_obj_set[tft_stt.objn][etn].value);
                tft_send_cmd(tft_cmd_str);
                break;
            case SW_PAGE:
                break;
            default:
                break;
            }
        }
        break;
    default:
        break;
    }
    return;
}

/**
 * sw_to_obj() - 将植物属性设置界面中的是否加入计划这一属性对应反馈到主界面中
 *
 * 在从植物属性设置界面返回主界面时调用
 */
static void sw_to_obj(void)
{
    for(uint8_t i = 0; i < 8; i++)
    {
        sprintf(tft_cmd_str, "vis obj%u,%u", i,
                        *get_value_of_kvp("sw", i));
        tft_send_cmd(tft_cmd_str);
    }
    return;
}




/**
 * get_value_of_kvp() - 得到一个页面元素的ID对应的vlaue的地址
 */
int16_t *get_value_of_kvp(char *name, uint8_t objn)
{
    // 遍历主界面
    for (int i = 0; i < sizeof(kvp_menu) / sizeof(kv_pair); i++)
    {
        if (strcmp(name, kvp_menu[i].key) == 0)
        {
            return &kvp_menu[i].value;
        }
    }
    // 遍历植物属性设置界面
    for (int i = 0; i < sizeof(kvp_obj_set[objn]) / sizeof(kv_pair); i++)
    {
        if (strcmp(name, kvp_obj_set[objn][i].key) == 0)
        {
            return &kvp_obj_set[objn][i].value;
        }
    }
    return 0;
}

/**
 * tft_input_limit() - 确定一个元素输入的区间
 * @name: 元素ID
 */
input_limit tft_input_limit(char *name)
{
    input_limit in_lmt;
    if (strcmp(name, "st_y") == 0)
    {
        in_lmt.min = 2000;
        in_lmt.max = 2100;
    }
    else if (strcmp(name, "st_mo") == 0)
    {
        in_lmt.min = 1;
        in_lmt.max = 12;
    }
    else if (strcmp(name, "st_d") == 0)
    {
        in_lmt.min = 1;
        in_lmt.max = get_month_days(*get_value_of_kvp("st_y", 0),
                *get_value_of_kvp("st_mo", 0));
    }
    else if (strcmp(name, "st_h") == 0)
    {
        in_lmt.min = 0;
        in_lmt.max = 23;
    }
    else if (strcmp(name, "st_mi") == 0)
    {
        in_lmt.min = 0;
        in_lmt.max = 59;
    }
    else if (strcmp(name, "st_s") == 0)
    {
        in_lmt.min = 0;
        in_lmt.max = 59;
    }
    else if (strcmp(name, "bg_y") == 0)
    {
        in_lmt.min = 2000;
        in_lmt.max = 2100;
    }
    else if (strcmp(name, "bg_mo") == 0)
    {
        in_lmt.min = 1;
        in_lmt.max = 12;
    }
    else if (strcmp(name, "bg_d") == 0)
    {
        in_lmt.min = 1;
        in_lmt.max = get_month_days(*get_value_of_kvp("bg_y", 0),
                *get_value_of_kvp("bg_mo", 0));
    }
    else if (strcmp(name, "bg_h") == 0)
    {
        in_lmt.min = 0;
        in_lmt.max = 23;
    }
    else if (strcmp(name, "bg_mi") == 0)
    {
        in_lmt.min = 0;
        in_lmt.max = 59;
    }
    else if (strcmp(name, "ed_h") == 0)
    {
        in_lmt.min = 0;
        in_lmt.max = 23;
    }
    else if (strcmp(name, "ed_mi") == 0)
    {
        in_lmt.min = 0;
        in_lmt.max = 59;
    }
    else if (strcmp(name, "pd_d") == 0)
    {
        in_lmt.min = 1;
        in_lmt.max = 30;
    }
    else if (strcmp(name, "pd_h") == 0)
    {
        in_lmt.min = 0;
        in_lmt.max = 23;
    }
    else if (strcmp(name, "pd_mi") == 0)
    {
        in_lmt.min = 0;
        in_lmt.max = 59;
    }
    else if (strcmp(name, "x") == 0)
    {
        in_lmt.min = 0;
        in_lmt.max = 359;
    }
    else if (strcmp(name, "y") == 0)
    {
        in_lmt.min = 0;
        in_lmt.max = 359;
    }
    else if (strcmp(name, "cnt") == 0)
    {
        in_lmt.min = 0;
        in_lmt.max = 1000;
    }
    else
    {
        in_lmt.min = 0;
        in_lmt.max = 0;
    }
    return in_lmt;
}











