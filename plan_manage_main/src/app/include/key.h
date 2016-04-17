/* key.h - 监测并采集按键的输入
 * 共有四种输入，无键，单击，双击，长击。
 * 使用一个定时器每10ms采集一次io状态来实现的。
 */

#ifndef KEY_H
#define KEY_H


typedef enum key_mean_
{
    N_KEY = 0, S_KEY, D_KEY, L_KEY
} key_mean;



/*
 * key_type - 按键的种类
 */
typedef enum key_type_
{
    UP_KEY = 0, DOWN_KEY, OK_KEY, RET_KEY, AMS_KEY, WIFI_KEY, MR_KEY, MB_KEY, MUVB_KEY
} key_type;



/*
 * key_init() - 按键模块初始化
 *
 */
void key_init(void);


/*
 * key_detect() - 检测按键
 *
 * 要求每隔10ms调用一次
 */
void key_detect(void);



/*
 * get_key_mean() - 得到指定的按键信息
 *
 * 这个函数中在取得信息后会将对应按键重新标记为N_KEY
 */
key_mean get_key_mean(key_type key_t);




void clear_key_m(void);




#endif // KEY_H
