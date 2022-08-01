#ifndef LCD_USER_H_
#define LCD_USER_H_
#include <stdint.h>
#include "stm32l0xx_hal.h"


#define Max_Column	128
#define Max_Row		64

void lcd_init(I2C_HandleTypeDef *hi2c);
void OLED_Set_Pos(unsigned char x, unsigned char y);
void set_poin(uint8_t x,uint8_t y,uint8_t state,uint8_t *p); 
void OLED_ShowChar(uint32_t x,uint32_t y,char chr,uint8_t mode);
void OLED_ShowBigChar(uint32_t x,uint32_t y,char chr,uint8_t mode);
void OLED_ShowString(uint32_t x,uint32_t y,char *chr,uint8_t mode);
void OLED_ShowBigString(uint32_t x,uint32_t y,char *chr,uint8_t mode);
void OLED_ShowStringXY(uint32_t x,uint32_t y,char *chr,uint8_t mode);
void OLED_ShowBigStringXY(uint32_t x,uint32_t y,char *chr,uint8_t mode);
void OLED_Clear(void);

#endif /* LCD_USER_H_ */
