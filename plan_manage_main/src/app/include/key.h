/* key.h - 监测并采集按键的输入
 * 共有四种输入，无键，单击，双击，长击。
 * 使用一个定时器每10ms采集一次io状态来实现的。
 */

#ifndef KEY_H
#define KEY_H


/*
 * key_mean - 按键的状态标志符号
 * N_KEY表示无键按下状态，S_KEY表示单击，D_KEY表示双击，L_KEY表示长按
 */
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




void key_init(void);
void key_detect(void);
key_mean get_key_mean(key_type key_t);
void clear_key_m(void);




#endif // KEY_H
