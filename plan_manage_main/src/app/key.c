/*
 * key.c - 监测并采集按键的输入
 *
 * 共有四种输入，无键，单击，双击，长击。
 * 每10ms需要采集一次io状态。
 * 每增减一个按键，需要设置的有:
 *
 *      key_type枚举类型中对应的枚举元素
 *
 *      对应的key_infomation变量
 *
 *      key_init()函数中对应的初始化条目
 *      key_detect()函数中对应检测条目
 *      get_key_mean()函数中对应获取条目
 */


#include <stdint.h>

#include "include.h"

#include "include/key.h"
#include "include/config.h"


/*
 * key_state - 表示按键的状态
 * key_state_0   初始状态，或者表示第一次单击状态
 * key_state_1   消抖和确认状态，或表示第二次单击状态
 * key_state_2   按键计时状态
 * key_state_3   按键等待释放状态
 */
typedef enum key_state_
{
    key_state_0 = 0, key_state_1, key_state_2, key_state_3
} key_state;

/*
 * IO状态符号定义
 */
typedef enum state_sign_
{
    INVALID = 0, VALID = 1
} state_sign;


/*
 * key_infomation - 表示一个按键的信息
 * state_base和time_base在key_read_base函数中使用的状态变量
 * state和time在key_read函数中使用的状态变量
 * key_m保存当前按键读取的结果
 * get_state_io变量指向获取按键对应输入io状态的函数
 */
typedef struct key_infomation_
{
    key_state state_base;
    uint8_t time_base;

    key_state state;
    uint8_t time;

    key_mean key_m;

    state_sign (*get_state_io)(void);
} key_infomation;



/*
 * 下边这几个函数定义的是获取按键io状态的函数
 */
state_sign get_state_up_key_io(void)
{ return !gpio_get(UP_KEY_PINX) ? VALID : INVALID; }

state_sign get_state_down_key_io(void)
{ return !gpio_get(DOWN_KEY_PINX) ? VALID : INVALID; }

state_sign get_state_ok_key_io(void)
{ return !gpio_get(OK_KEY_PINX) ? VALID : INVALID; }

state_sign get_state_ret_key_io(void)
{ return !gpio_get(RET_KEY_PINX) ? VALID : INVALID; }

state_sign get_state_ams_key_io(void)
{ return !gpio_get(AMS_KEY_PINX) ? VALID : INVALID; }

state_sign get_state_wifi_key_io(void)
{ return !gpio_get(WIFI_KEY_PINX) ? VALID : INVALID; }

state_sign get_state_mr_key_io(void)
{ return !gpio_get(MR_KEY_PINX) ? VALID : INVALID; }

state_sign get_state_mb_key_io(void)
{ return !gpio_get(MB_KEY_PINX) ? VALID : INVALID; }

state_sign get_state_muvb_key_io(void)
{ return !gpio_get(MUVB_KEY_PINX) ? VALID : INVALID; }


// 定义按键变量并初始化
static volatile key_infomation
up_key_info =   { key_state_0, 0, key_state_0, 0, N_KEY, get_state_up_key_io },
down_key_info = { key_state_0, 0, key_state_0, 0, N_KEY, get_state_down_key_io },
ok_key_info =   { key_state_0, 0, key_state_0, 0, N_KEY, get_state_ok_key_io },
ret_key_info =  { key_state_0, 0, key_state_0, 0, N_KEY, get_state_ret_key_io },
ams_key_info =  { key_state_0, 0, key_state_0, 0, N_KEY, get_state_ams_key_io },
wifi_key_info = { key_state_0, 0, key_state_0, 0, N_KEY, get_state_wifi_key_io },
mr_key_info =   { key_state_0, 0, key_state_0, 0, N_KEY, get_state_mr_key_io },
mb_key_info =   { key_state_0, 0, key_state_0, 0, N_KEY, get_state_mb_key_io },
muvb_key_info = { key_state_0, 0, key_state_0, 0, N_KEY, get_state_muvb_key_io };



/**
 * key_init() - 按键模块初始化
 *
 */
void key_init(void)
{
    gpio_Interrupt_init(UP_KEY_PINX, GPI_UP_PF, GPI_DISAB);
    gpio_Interrupt_init(DOWN_KEY_PINX, GPI_UP_PF, GPI_DISAB);
    gpio_Interrupt_init(OK_KEY_PINX, GPI_UP_PF, GPI_DISAB);
    gpio_Interrupt_init(RET_KEY_PINX, GPI_UP_PF, GPI_DISAB);
    gpio_Interrupt_init(AMS_KEY_PINX, GPI_UP_PF, GPI_DISAB);
    gpio_Interrupt_init(WIFI_KEY_PINX, GPI_UP_PF, GPI_DISAB);
    gpio_Interrupt_init(MR_KEY_PINX, GPI_UP_PF, GPI_DISAB);
    gpio_Interrupt_init(MB_KEY_PINX, GPI_UP_PF, GPI_DISAB);
    gpio_Interrupt_init(MUVB_KEY_PINX, GPI_UP_PF, GPI_DISAB);

    return;
}


/**
 * key_read_base - 按键基础读取函数，
 * @key_info: 要检测按键的所需信息,必须在是全局变量区中存储的
 *
 * 这个函数返回 N_KEY,S_KEY,L_KEY三种情况
 */
key_mean key_read_base(volatile key_infomation *key_info)
{
    state_sign key_state_io;
    key_mean key_m = N_KEY;

    // 获取io状态
    key_state_io = (*(key_info->get_state_io))();
    switch(key_info->state_base)
    {
    case key_state_0:
        if (key_state_io == VALID)
        {
            // 按键按下后首先进入消抖和按键确认状态
            key_info->state_base = key_state_1;
        }
        break;
    case key_state_1:
        if (key_state_io == VALID)
        {
            // 按键如果仍然按下，则消抖完成
            // 进行状态转换,并且返回无按键事件
            key_info->time_base = 0;
            key_info->state_base = key_state_2;
        }
        else
        {
            key_info->state_base = key_state_0;
        }
        break;
    case key_state_2:
        if (key_state_io == INVALID)
        {
            // 此时按键释放说明为一次短操作
            key_m = S_KEY;
            key_info->state_base = key_state_0;
        }
        // 继续按下，计时加10ms，即本函数调用周期
        else if (++key_info->time_base > 100)
        {
            // 按下时间大于1s，返回长按键，并且转换为等待按键释放状态
            key_m = L_KEY;
            key_info->state_base = key_state_3;
        }
        break;
    case key_state_3:
        if (key_state_io == INVALID)
        {
            // 按键此时已释放，转换为初始状态
            key_info->state_base = key_state_0;
        }
        break;
    default:
        break;
    }
    return key_m;
}

/**
 * key_read - 按键读取函数
 * @key_info: 要检测的按键所需的信息，必须是在全局变量区中存储的
 *
 * 这个函数会返回N_KEY,S_KEY,D_KEY，L_KEY四情况
 */
key_mean key_read(volatile key_infomation *key_info)
{
    key_mean key_m = N_KEY,
            key_m_temp;

    key_m_temp = key_read_base(key_info);
    switch(key_info->state)
    {
    case key_state_0:
        if (key_m_temp == S_KEY)
        {
            // 第一次单击状态，仍然返回无键，到下个周期判断是否出现双击
            key_info->time = 0;
            key_info->state = key_state_1;
        }
        else
        {
            // 对于非单击，返回原事件
            key_m = key_m_temp;
        }
        break;
    case key_state_1:
        // 又一次单击，间隔 < 300ms
        if (key_m_temp == S_KEY)
        {
            key_m = D_KEY;
            key_info->state = key_state_0;
        }
        else
        {
            if (++key_info->time > 30)
            {
                // 300ms内没有再出现单击事件，则返回上一次单击事件
                key_m = S_KEY;
                key_info->state = key_state_0;
            }
        }
        break;
    default:
        break;
    }
    return key_m;
}


/**
 * key_detect() - 检测按键
 *
 * 这个函数仅在N_KEY状态时记录按键信息，N_KEY是在应用程序获取按键信息的时候同时
 * 被设置的，这样做的目的是保证采集到的按键一定会被执行
 * 要求每隔10ms调用一次
 */
void key_detect(void)
{
    key_mean key_m;

    if ((key_m = key_read(&up_key_info)) != N_KEY)
    {
        up_key_info.key_m = key_m;
    }
    if ((key_m = key_read(&down_key_info)) != N_KEY)
    {
        down_key_info.key_m = key_m;
    }
    if ((key_m = key_read(&ok_key_info)) != N_KEY)
    {
        ok_key_info.key_m = key_m;
    }
    if ((key_m = key_read(&ret_key_info)) != N_KEY)
    {
        ret_key_info.key_m = key_m;
    }
    if ((key_m = key_read(&ams_key_info)) != N_KEY)
    {
        ams_key_info.key_m = key_m;
    }
    if ((key_m = key_read(&wifi_key_info)) != N_KEY)
    {
        wifi_key_info.key_m = key_m;
    }
    if ((key_m = key_read(&mr_key_info)) != N_KEY)
    {
        mr_key_info.key_m = key_m;
    }
    if ((key_m = key_read(&mb_key_info)) != N_KEY)
    {
        mb_key_info.key_m = key_m;
    }
    if ((key_m = key_read(&muvb_key_info)) != N_KEY)
    {
        muvb_key_info.key_m = key_m;
    }
    return;
}


/**
 * get_key_mean() - 得到指定的按键信息
 *
 * 这个函数中在取得信息后会将对应按键重新标记为N_KEY
 */
key_mean get_key_mean(key_type key_t)
{
    key_mean key_m;

    switch (key_t)
    {
    case UP_KEY:
        key_m = up_key_info.key_m;
        up_key_info.key_m = N_KEY;
        break;
    case DOWN_KEY:
        key_m = down_key_info.key_m;
        down_key_info.key_m = N_KEY;
        break;
    case OK_KEY:
        key_m = ok_key_info.key_m;
        ok_key_info.key_m = N_KEY;
        break;
    case RET_KEY:
        key_m = ret_key_info.key_m;
        ret_key_info.key_m = N_KEY;
        break;
    case AMS_KEY:
        key_m = ams_key_info.key_m;
        ams_key_info.key_m = N_KEY;
        break;
    case WIFI_KEY:
        key_m = wifi_key_info.key_m;
        wifi_key_info.key_m = N_KEY;
        break;
    case MR_KEY:
        key_m = mr_key_info.key_m;
        mr_key_info.key_m = N_KEY;
        break;
    case MB_KEY:
        key_m = mb_key_info.key_m;
        mb_key_info.key_m = N_KEY;
        break;
    case MUVB_KEY:
        key_m = muvb_key_info.key_m;
        muvb_key_info.key_m = N_KEY;
        break;
    default:
        break;
    }
    return key_m;
}


void clear_key_m(void)
{
    down_key_info.key_m = N_KEY;
    up_key_info.key_m = N_KEY;
    ok_key_info.key_m = N_KEY;
    ret_key_info.key_m = N_KEY;
    ams_key_info.key_m = N_KEY;
    wifi_key_info.key_m = N_KEY;
    mr_key_info.key_m = N_KEY;
    mb_key_info.key_m = N_KEY;
    muvb_key_info.key_m = N_KEY;
}
