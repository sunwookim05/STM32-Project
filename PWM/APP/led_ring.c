#include "led_ring.h"
#include "main.h"
volatile uint8_t datasentflag;

uint8_t LED_Data[MAX_LED][4];
uint8_t LED_Mod[MAX_LED][4];  // for brightness

void Set_LED(int LEDnum, int Red, int Green, int Blue) {
	LED_Data[LEDnum][0] = LEDnum;
	LED_Data[LEDnum][1] = Green;
	LED_Data[LEDnum][2] = Red;
	LED_Data[LEDnum][3] = Blue;
}

uint16_t pwmData[(24 * MAX_LED) + 50];

void WS2812_Send(void) {
	uint32_t indx = 0;
	uint32_t color;

	for (int i = 0; i < MAX_LED; i++) {
         color = ((LED_Data[i][1]<<16) | (LED_Data[i][2]<<8) | (LED_Data[i][3]));

		for (int i = 23; i >= 0; i--) {
			if (color & (1 << i)) {
				pwmData[indx] = 40 * 2 / 3;
			} else {
				pwmData[indx] = 40 * 1 / 3;
			}
			indx++;
		}

	}

	for (int i = 0; i < 50; i++) {
		pwmData[indx] = 0;
		indx++;
	}

	HAL_TIM_PWM_Start_DMA(&htim2, TIM_CHANNEL_1, (uint32_t*) pwmData, indx);
	while (!datasentflag) {};
	datasentflag = 0;
}

void HAL_TIM_PWM_PulseFinishedCallback(TIM_HandleTypeDef *htim) {
	HAL_TIM_PWM_Stop_DMA(&htim2, TIM_CHANNEL_1);
	datasentflag = 1;
}
