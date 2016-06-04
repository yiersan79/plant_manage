/*
 * tft.h - tft½Ó¿Ú
 */


#ifndef TFT_H
#define TFT_H


typedef struct input_limit_
{
    int16_t max;
    int16_t min;
} input_limit;

void tft_init(void);

void tft_send_cmd(const char *cmd);

void tft_left(void);

void tft_right(void);

void tft_up(void);

void tft_down(void);

void tft_ok(void);

void tft_ret(void);

void tft_page_refresh(void);

int16_t *get_value_of_kvp(char *name, uint8_t objn);

input_limit tft_input_limit(char *name);





#endif /* TFT_H */
