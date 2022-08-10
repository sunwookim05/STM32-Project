#include <stdio.h>
#include <stdlib.h>
#include "app.h"
#include "lcd1602.h"

#define SW1 HAL_GPIO_ReadPin(SW_ON_GPIO_Port, SW_ON_Pin)
#define SW2 HAL_GPIO_ReadPin(SW_AUTO_GPIO_Port, SW_LOCK_Pin)
#define SW3 HAL_GPIO_ReadPin(SW_AUTO_GPIO_Port, SW_AUTO_Pin)
#define SW4 HAL_GPIO_ReadPin(SW_A_GPIO_Port, SW_A_Pin)
#define SW5 HAL_GPIO_ReadPin(SW_B_GPIO_Port, SW_B_Pin)
#define BUZZER(X) HAL_GPIO_WritePin(BUZZ_GPIO_Port, BUZZ_Pin, X)
#define LED(N, X) HAL_GPIO_WritePin(LED##N##_GPIO_Port, LED##N##_Pin, !X)
#define NOW HAL_GetTick()
#define TEMPUP(X) (X == 1 ? 900 : X == 2 ? 800 : X == 3 ? 700 : X == 4 ? 600 : X == 5 ? 500 : X == 6 ? 400 : X == 7 ? 300 : X == 8 ? 200 : X == 9 ? 100 : 0)
#define TEMPDOWN(X) (X < 10 ? 2900 : X >= 300 ? 100 : X >= 200 ? 200 : X >= 100 ? 400 : X >= 40 ? 700 : X >= 20 ? 1100 : X >= 15 ? 1600 : X >= 10 ? 2200 : 0)
#define LEDCLEAR LED(1, false); LED(2, false); LED(3, false); LED(4, false); LED(5, false);

boolean swFlag = false;
boolean i = false;
uint32_t last;
String bool[2] = { "false", "true "};
String str;

void setUp() {
	str = malloc(sizeof(char) * 16);
	LcdInit();
	for (int i = 0; i < 12; i++) {
		Set_LED(i, 0, 0, 0);
		WS2812_Send();
	}
}

void appLoop() {
	lcd_gotoxy(0, 1);
	sprintf(str, "Hello World!");
	lcd_puts(str);
	lcd_gotoxy(0, 0);
	sprintf(str, "%s",bool[i]);
	lcd_puts(str);
	if (NOW - last >= 10) {
		if (!SW3) {
			if(!swFlag){
				i = !i;
				swFlag = true;
			}
		}else{
			swFlag = false;
		}
		last = NOW;
	}
}
