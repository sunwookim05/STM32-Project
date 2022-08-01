#include "main.h"
#include "App.h"

#define LED1(x) (HAL_GPIO_WritePin(LED1_GPIO_Port, LED1_Pin, x))
#define LED2(x) (HAL_GPIO_WritePin(LED2_GPIO_Port, LED2_Pin, x))
#define NOW (HAL_GetTick())

extern TIM_HandleTypeDef htim6;

void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin) {
	if (GPIO_Pin == S1_Pin) {
		HAL_GPIO_TogglePin(LED1_GPIO_Port, LED1_Pin);
	}
}

void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim) {
	if(htim == &htim6){
		HAL_GPIO_TogglePin(LED1_GPIO_Port, LED1_Pin);
	}
}

void appLoop() {

	static uint32_t last;

	if (NOW - last > 500) {
		HAL_GPIO_TogglePin(LED2_GPIO_Port, LED2_Pin);
		last = NOW;
	}

}
