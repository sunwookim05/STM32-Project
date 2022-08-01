/*
 * App.c
 *
 *  Created on: Dec 25, 2021
 *      Author: LG
 */

#include <stdio.h>
#include <stdbool.h>

#include "main.h"
#include "App.h"
#include "lcd_user.h"

int32_t computeTemperature(uint32_t m);

extern I2C_HandleTypeDef hi2c1;
extern RTC_HandleTypeDef hrtc;

RTC_DateTypeDef sDate;
RTC_TimeTypeDef sTime;

void appSetup() {

	lcd_init(&hi2c1);
	OLED_ShowString(1, 1, "Starting...", 0);
	HAL_Delay(4000);
}
void appLoop() {

	static uint32_t last;
	uint32_t now = HAL_GetTick();

	if (now - last > 1000) {
		HAL_GPIO_TogglePin(LED1_GPIO_Port, LED1_Pin);

		HAL_RTC_GetTime(&hrtc, &sTime, RTC_FORMAT_BIN);
		HAL_RTC_GetDate(&hrtc, &sDate, RTC_FORMAT_BIN);
		char strBuf[40];
		sprintf(strBuf, "%02d-%02d-%02d %02d:%02d:%02d", sDate.Year, sDate.Month, sDate.Date, sTime.Hours, sTime.Minutes, sTime.Seconds);
		//OLED_Clear();
		OLED_ShowString(1, 1, strBuf, 1);

		last = now;
	}

}
