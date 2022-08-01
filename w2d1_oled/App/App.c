#include <stdbool.h>
#include <stdio.h>
#include "main.h"
#include "lcd_user.h"

#define LED1(X) (HAL_GPIO_WritePin(LED1_GPIO_Port, LED1_Pin, (X)))
#define LED4(X) (HAL_GPIO_WritePin(LED4_GPIO_Port, LED4_Pin, (X)))
#define BUZZER(X) (HAL_GPIO_WritePin(BUZZER_GPIO_Port, BUZZER_Pin, (X)))
#define NOW (HAL_GetTick())

extern I2C_HandleTypeDef hi2c1;
extern ADC_HandleTypeDef hadc;
extern DMA_HandleTypeDef hdma_adc;

volatile bool updateAdcValues = false;
static uint32_t adc_val[3];

void HAL_ADC_ConvCpltCallback(ADC_HandleTypeDef *hadc) {
	updateAdcValues = true;
	HAL_GPIO_TogglePin(LED4_GPIO_Port, LED4_Pin);
}

void appLoop() {
	static uint32_t last;

//	HAL_ADC_Start(&hadc);
//	HAL_ADC_PollForConversion(&hadc, 5);
//	uint32_t adc_val = HAL_ADC_GetValue(&hadc);

	if (NOW - last > 500) {
		HAL_GPIO_TogglePin(LED1_GPIO_Port, LED1_Pin);
		HAL_ADC_Start_DMA(&hadc, adc_val, 3);
		last = NOW;
	}

	if (updateAdcValues) {
		updateAdcValues = false;
		uint32_t lAdcValues[3];
		char strBuffer[40] = { 0, };
		__disable_irq();
		for (int i = 0; i < 3; i++) {
			lAdcValues[i] = adc_val[i];
		}
		__enable_irq();
		for (int i = 0; i < 3; i++) {
			sprintf(strBuffer, "%d: %04d", i, lAdcValues[i]);
			OLED_ShowBigString(0, i*2, strBuffer, 1);
		}
		BUZZER(lAdcValues[0] > 2048);
	}

//	HAL_ADC_Stop(&hadc);

}
