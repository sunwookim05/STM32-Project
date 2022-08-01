#include "main.h"
#include <string.h>
#include <stdarg.h>
#include <stdint.h>

#define LCD_RS(x) HAL_GPIO_WritePin(LCD_RS_GPIO_Port, LCD_RS_Pin, (GPIO_PinState)x)
#define LCD_RW(x) HAL_GPIO_WritePin(LCD_RW_GPIO_Port, LCD_RW_Pin, (GPIO_PinState)x)
#define LCD_EN(x) HAL_GPIO_WritePin(LCD_EN_GPIO_Port, LCD_EN_Pin, (GPIO_PinState)x)
#define LCD_D4(x) HAL_GPIO_WritePin(LCD_D4_GPIO_Port, LCD_D4_Pin, (GPIO_PinState)x)
#define LCD_D5(x) HAL_GPIO_WritePin(LCD_D5_GPIO_Port, LCD_D5_Pin, (GPIO_PinState)x)
#define LCD_D6(x) HAL_GPIO_WritePin(LCD_D6_GPIO_Port, LCD_D6_Pin, (GPIO_PinState)x)
#define LCD_D7(x) HAL_GPIO_WritePin(LCD_D7_GPIO_Port, LCD_D7_Pin, (GPIO_PinState)x)

const uint8_t LCD_INIT_STRING[4] = {0x28, 0x0C, 0x01, 0x06};

uint8_t LcdReadByte() {
  GPIO_InitTypeDef GPIO_InitStruct;
  uint8_t lcdBusy;
  
  GPIO_InitStruct.Pin = LCD_D7_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(LCD_D7_GPIO_Port, &GPIO_InitStruct);
  
  LCD_RW(1); LCD_EN(1);
  lcdBusy = HAL_GPIO_ReadPin(LCD_D7_GPIO_Port, LCD_D7_Pin);
  
  LCD_EN(0); LCD_EN(1); LCD_EN(0);
  GPIO_InitStruct.Pin = LCD_D7_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
  HAL_GPIO_Init(LCD_D7_GPIO_Port, &GPIO_InitStruct);
  
  return lcdBusy;
}

void LcdSendNibble(uint8_t n) {
  LCD_D4(n >> 0 & 1);
  LCD_D5(n >> 1 & 1);
  LCD_D6(n >> 2 & 1);
  LCD_D7(n >> 3 & 1);
  LCD_EN(1); LCD_EN(0);
}

void LcdSendByte(uint8_t address, uint8_t n) {
  LCD_RS(0);
  while(LcdReadByte());
  LCD_RS(address);
  LCD_RW(0); LCD_EN(0);
  LcdSendNibble(n >> 4);
  LcdSendNibble(n & 0x0F);
}

void LcdInit() {
  uint8_t i;
  LCD_RS(0); LCD_RW(0); LCD_EN(0);
  HAL_Delay(15);
  for(i=0; i<3; i++) {
    LcdSendNibble(3);
    HAL_Delay(5);
  }
  LcdSendNibble(2);
  for(i=0; i<4; i++) LcdSendByte(0, LCD_INIT_STRING[i]);
}

void lcd_gotoxy(uint8_t x, uint8_t y) {
  uint8_t address;
  if(y != 1) address = 0x40;
  else address = 0;
  address += x-1;
  LcdSendByte(0, 0x80 | address);
}

void lcd_putc(uint8_t c) {
  if(c == '\f') {
    LcdSendByte(0, 1);
    HAL_Delay(2);
  }
  else if(c == '\n') lcd_gotoxy(1, 2);
  else if(c == '\b') LcdSendByte(0, 0x10);
  else LcdSendByte(1, c);
}

void lcd_puts(const char* str) {
  uint8_t i;

  for(i=0; i<strlen(str); i++) {
    lcd_putc(str[i]);
  }
}

void lcd_cgram(uint8_t room, uint8_t n) {
  uint8_t i;
  const uint8_t data[2][8]={{0x04, 0x04, 0x0E, 0x0F, 0x1B, 0x19, 0x11, 0x0A}, // FIRE SYMBOL
                            {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}}; //
  
  for(i=0; i<8; i++) {
    LcdSendByte(0, 0x40+i+(room*8));
    LcdSendByte(1, data[n][i]);
  }
}
