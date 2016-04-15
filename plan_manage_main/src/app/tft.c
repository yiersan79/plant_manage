/*
 * tft.c - tft 显示屏的接口
 *
 * uart hmi方式
 */

#include <stdint.h>
#include <string.h>
#include <stdio.h>

#include "uart.h"

#include "include/tft.h"
#include "include/knob.h"
#include "include/key.h"
#include "include/config.h"


typedef struct kv_pair_
{
    char *key;
    int16_t value;
    int8_t attr;
} kv_pair;

typedef struct tft_state_
{
    uint8_t pgn;
    uint8_t etn;
    uint8_t ln;
    uint8_t objn;
} tft_state;

typedef enum page_name_
{
    ORIGINAL_PG = 0, MENU_PG, OBJ_SET_PG
} page_name;

typedef enum entry_attr_
{
    R_NUM = 0, RW_NUM, RW_PIC, SW_PAGE
} entry_attr;


static tft_state tft_stt = { 0, 0, 0, 0 };

static uint8_t original_lyt[] = { 0, 1 };
static uint8_t menu_lyt[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 11 };
static uint8_t obj_set_lyt[] = { 0, 4, 8, 12, 13, 16, 18, 19 };

kv_pair kvp_menu[] = {
    { "st_y", 2016, RW_NUM }, { "st_mo", 4, RW_NUM }, { "st_d", 14, RW_NUM }, { "st_mi", 0, RW_NUM },
    { "p1", 0, RW_PIC },
    { "p2", 0, RW_PIC },
    { "p3", 0, RW_PIC },
    { "p4", 0, RW_PIC },
    { "p5", 0, RW_PIC },
    { "note", 0, RW_NUM }
};

kv_pair kvp_obj_set[][19] = 
{
    {
        { "st_y", 2016, RW_NUM }, { "st_mo", 4, RW_NUM }, { "st_d", 14, RW_NUM }, { "st_mi", 0, RW_NUM },
        { "bg_y", 2016, RW_NUM }, { "bg_mo", 4, RW_NUM }, { "bg_d", 14, RW_NUM }, { "bg_mi", 0, RW_NUM },
        { "ed_y", 2016, RW_NUM }, { "ed_mo", 4, RW_NUM }, { "ed_d", 14, RW_NUM }, { "ed_mi", 0, RW_NUM },
        { "pd", 3, RW_NUM },
        { "lg_r", 0, RW_PIC}, { "lg_b", 0, RW_PIC }, { "lg_uvb", 0, RW_PIC },
        { "lg_cnt", 0, RW_NUM }, { "wt_cnt", 0, RW_NUM },
        { "note", 0, RW_NUM }
    },

    {
        { "st_y", 2016, RW_NUM }, { "st_mo", 4, RW_NUM }, { "st_d", 14, RW_NUM }, { "st_mi", 0, RW_NUM },
        { "bg_y", 2016, RW_NUM }, { "bg_mo", 4, RW_NUM }, { "bg_d", 14, RW_NUM }, { "bg_mi", 0, RW_NUM },
        { "ed_y", 2016, RW_NUM }, { "ed_mo", 4, RW_NUM }, { "ed_d", 14, RW_NUM }, { "ed_mi", 0, RW_NUM },
        { "pd", 3, RW_NUM },
        { "lg_r", 0, RW_PIC}, { "lg_b", 0, RW_PIC }, { "lg_uvb", 0, RW_PIC },
        { "lg_cnt", 0, RW_NUM }, { "wt_cnt", 0, RW_NUM },
        { "note", 0, RW_NUM }
    }
};

static char tft_cmd_str[10];

/*
 *
 */ 
void tft_init(void)
{
    uart_init(TFT_UARTX, 9600);
    return;
}

/*
 *
 */
void tft_send_cmd(const char *cmd)
{
    uart_sendStr(TFT_UARTX, (const uint8_t *)cmd);

    uart_putchar(TFT_UARTX, 0xff);
    uart_putchar(TFT_UARTX, 0xff);
    uart_putchar(TFT_UARTX, 0xff);
}

/*
 *
 */
void tft_up(void)
{
    switch (tft_stt.pgn)
    {
    case ORIGINAL_PG:
        if (tft_stt.ln > 0)
        {
            tft_stt.ln--;
        }
        tft_stt.etn  = original_lyt[tft_stt.ln];
        break;
    case MENU_PG:
        if (tft_stt.ln > 0)
        {
            tft_stt.ln--;
        }
        tft_stt.etn  = menu_lyt[tft_stt.ln];
        break;
    case OBJ_SET_PG:
        if (tft_stt.ln > 0)
        {
            tft_stt.ln--;
        }
        tft_stt.etn  = obj_set_lyt[tft_stt.ln];
        break;
    default:
        break;
    }
    return;
}

void tft_down(void)
{
    switch (tft_stt.pgn)
    {
    case ORIGINAL_PG:
        if (tft_stt.ln < sizeof(original_lyt) / sizeof(uint8_t) - 1)
        {
            tft_stt.ln++;
        }
        tft_stt.etn  = original_lyt[tft_stt.ln];
        break;
    case MENU_PG:
        if (tft_stt.ln < sizeof(menu_lyt) / sizeof(uint8_t) - 1)
        {
            tft_stt.ln++;
        }
        tft_stt.etn  = menu_lyt[tft_stt.ln];
        break;
    case OBJ_SET_PG:
        if (tft_stt.ln < sizeof(obj_set_lyt) / sizeof(uint8_t) - 1)
        {
            tft_stt.ln++;
        }
        tft_stt.etn  = obj_set_lyt[tft_stt.ln];
        break;

    }
    return;
}

void tft_left(void)
{
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
        break;
    case MENU_PG:
        if (tft_stt.etn > 0)
        {
            if (--tft_stt.etn < menu_lyt[tft_stt.ln])
            {
                tft_stt.ln--;
            }
        }
        break;
    case OBJ_SET_PG:
        if (tft_stt.etn > 0)
        {
            if (--tft_stt.etn < obj_set_lyt[tft_stt.ln])
            {
                tft_stt.ln--;
            }
        }
        break;
    default:
        break;
    }
    return;
}

void tft_right(void)
{
    switch (tft_stt.pgn)
    {
    case ORIGINAL_PG:
        if (tft_stt.etn < original_lyt[sizeof(original_lyt) / sizeof(uint8_t) - 1] - 1)
        {
            if (++tft_stt.etn >= original_lyt[tft_stt.ln + 1])
            {
                tft_stt.ln++;
            }
        }
        break;
    case MENU_PG:
        if (tft_stt.etn < menu_lyt[sizeof(menu_lyt) / sizeof(uint8_t) - 1] - 1)
        {
            if (++tft_stt.etn >= menu_lyt[tft_stt.ln + 1])
            {
                tft_stt.ln++;
            }
        }
        break;
    case OBJ_SET_PG:
        if (tft_stt.etn < obj_set_lyt[sizeof(obj_set_lyt) / sizeof(uint8_t) - 1] - 1)
        {
            if (++tft_stt.etn >= obj_set_lyt[tft_stt.ln + 1])
            {
                tft_stt.ln++;
            }
        }
        break;
    default:
        break;
    }
    return;
}

void refrush_obj(void)
{
    for (int i = 0; i < sizeof(kvp_obj_set[tft_stt.objn]) / sizeof(kv_pair); i++)
    {
        switch (kvp_obj_set[tft_stt.objn][tft_stt.etn].attr)
        {
        case R_NUM:
        sprintf(tft_cmd_str,"%s.txt=\"%d\"", 
                kvp_obj_set[tft_stt.objn][tft_stt.etn].key,
                kvp_obj_set[tft_stt.objn][tft_stt.etn].value);
        tft_send_cmd(tft_cmd_str);
            break;
        case RW_NUM:
        sprintf(tft_cmd_str,"%s.txt=\"%d\"", 
                kvp_obj_set[tft_stt.objn][tft_stt.etn].key,
                kvp_obj_set[tft_stt.objn][tft_stt.etn].value);
        tft_send_cmd(tft_cmd_str);
            break;
        case RW_PIC:
        sprintf(tft_cmd_str, "vis %s,%d", 
                kvp_obj_set[tft_stt.objn][tft_stt.etn].key,
                kvp_obj_set[tft_stt.objn][tft_stt.etn].value);
        tft_send_cmd(tft_cmd_str);
            break;
        case SW_PAGE:
            break;
        default:
            break;
        }
    }
    return;
}

void refrush_menu(void)
{
    for (int i = 0; i < sizeof(kvp_obj_set[tft_stt.objn]) / sizeof(kv_pair); i++)
    {
        
    }
    return;
}

void tft_ret(void)
{
    switch (tft_stt.pgn)
    {
    case ORIGINAL_PG:
        break;
    case MENU_PG:
        tft_stt.pgn = ORIGINAL_PG;
        tft_stt.ln = 0;
        tft_stt.etn = 0;
        tft_send_cmd("page original");
        break;
    case OBJ_SET_PG:
        tft_stt.pgn = MENU_PG;
        tft_stt.ln = 0;
        tft_stt.etn = 0;
        tft_send_cmd("page menu");
        refrush_menu();
        break;
    default:
        break;
    }
    return;
}


void tft_ok(void)
{
    switch (tft_stt.pgn)
    {
    case ORIGINAL_PG:
        switch (kvp_obj_set[tft_stt.objn][tft_stt.etn].attr)
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
        case RW_NUM:
            knob_enable();
            while (get_key_mean(RET_KEY) == N_KEY)
            {
                kvp_menu[tft_stt.etn].value += get_knob_val();
                sprintf(tft_cmd_str, "%s.txt=\"%d\"", kvp_menu[tft_stt.etn].key,
                        kvp_menu[tft_stt.etn].value);
                tft_send_cmd(tft_cmd_str);
            }
            knob_disable();
            break;
        case RW_PIC:
            while (get_key_mean(RET_KEY) == N_KEY)
            {
                if (get_key_mean(UP_KEY) == S_KEY || get_key_mean(DOWN_KEY) == S_KEY)
                {
                    kvp_menu[tft_stt.etn].value = !kvp_menu[tft_stt.etn].value;
                    sprintf(tft_cmd_str, "vis %s,%d", kvp_menu[tft_stt.etn].key,
                            kvp_menu[tft_stt.etn].value);
                    tft_send_cmd(tft_cmd_str);
                }
            }
            break;
        case SW_PAGE:
            tft_stt.pgn = OBJ_SET_PG;
            tft_stt.objn = kvp_menu[tft_stt.etn].value;
            tft_stt.ln = 0;
            tft_stt.etn = 0;
            tft_send_cmd("page obj_set");
            refrush_obj();
            break;
        default:
            break;
        }
    case OBJ_SET_PG:
        switch (kvp_obj_set[tft_stt.objn][tft_stt.etn].attr)
        {
        case R_NUM:
            break;
        case RW_NUM:
            knob_enable();
            while (get_key_mean(RET_KEY) == N_KEY)
            {
                kvp_obj_set[tft_stt.objn][tft_stt.etn].value += get_knob_val();
                sprintf(tft_cmd_str,"%s.txt=\"%d\"", 
                        kvp_obj_set[tft_stt.objn][tft_stt.etn].key,
                        kvp_obj_set[tft_stt.objn][tft_stt.etn].value);
                tft_send_cmd(tft_cmd_str);
            }
            knob_disable();
            break;
        case RW_PIC:
            while (get_key_mean(RET_KEY) == N_KEY)
            {
                if (get_key_mean(UP_KEY) == S_KEY || get_key_mean(DOWN_KEY))
                {
                    kvp_obj_set[tft_stt.objn][tft_stt.etn].value =
                        !kvp_obj_set[tft_stt.objn][tft_stt.etn].value;
                    sprintf(tft_cmd_str, "vis %s,%d", 
                            kvp_obj_set[tft_stt.objn][tft_stt.etn].key,
                            kvp_obj_set[tft_stt.objn][tft_stt.etn].value);
                    tft_send_cmd(tft_cmd_str);
                }
            }
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

