#include "main.h"

#define LED1(x) (HAL_GPIO_WritePin(LED1_GPIO_Port, LED1_Pin, x))
#define SW1 (HAL_GPIO_ReadPin(S1_GPIO_Port, S1_Pin))



void appLoop() {
#define BSP_TICKS_PER_SEC 1000
	static enum {
		INIT, OFF_STATE, ON_STATE
	} state = INIT;

	static uint32_t start;

	uint32_t now = HAL_GetTick();

	switch (state) {
	case INIT:
		start = now;
		state = OFF_STATE;
		HAL_GPIO_WritePin(LED1_GPIO_Port, LED1_Pin, 1U);
		break;
	case OFF_STATE:
		if (now - start > BSP_TICKS_PER_SEC * 1U / 4U) {
			start = now;
			state = ON_STATE;
			HAL_GPIO_WritePin(LED1_GPIO_Port, LED1_Pin, 1);
		}
		break;
	case ON_STATE:
		if (now - start > BSP_TICKS_PER_SEC * 3U / 4U) {
			start = now;
			state = OFF_STATE;
			HAL_GPIO_WritePin(LED1_GPIO_Port, LED1_Pin, 0);
		}
		break;
	default:
		break;
	}
}
