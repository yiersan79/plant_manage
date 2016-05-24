#include <stdint.h>

#include "adc.h"

#include "config.h"
#include "water.h"





void water_init(void)
{
    gpio_init(WATER_PINX, 1, 1);
    adc_init(MOISTURE_ADC0_CHN);
    return;
}


uint16_t get_moisture(void)
{
    uint16_t res;

    res = adc_ave(MOISTURE_ADC0_CHN, ADC_16bit, 3);
    return (uint16_t)((float)res / 0xffff * MOISTURE_RATIO + MOISTURE_MIN);
}

void water_ctr(uint16_t sv_moist)
{
    uint16_t crt_moist = get_moisture();

    if (sv_moist > crt_moist)
    {
        gpio_set(WATER_PINX, 0);
    }
    else
    {
         gpio_set(WATER_PINX, 1);
    }
    return;
}
