/*
 * water.h - ½½Ë®
 */


#ifndef WARER_H
#define WARER_H


#define MOISTURE_RATIO  0.1
#define MOISTURE_MIN    1




void water_init(void);
uint16_t get_moisture(void);
void water_ctr(uint16_t sv_moist);









#endif /* WARER_H */
