/*
 * pm_flash.h - flash应用模块
 */

#ifndef PM_FLASH_H
#define PM_FLASH_H



void pm_flash_init(void);



void flash_write(uint8_t *saddr, uint16_t nbyte);


void flash_read(uint8_t *daddr, uint8_t nbyte);








#endif /* PM_FLASH_H */
