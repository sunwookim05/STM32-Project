#include "main.h"
#include <stdbool.h>

#define LED1(x) (HAL_GPIO_WritePin(LED1_GPIO_Port, LED1_Pin, (x)))
#define LED2(x) (HAL_GPIO_WritePin(LED2_GPIO_Port, LED2_Pin, (x)))
#define LED3(x) (HAL_GPIO_WritePin(LED3_GPIO_Port, LED3_Pin, (x)))
#define LED4(x) (HAL_GPIO_WritePin(LED4_GPIO_Port, LED4_Pin, (x)))

extern ADC_HandleTypeDef hadc;
extern DMA_HandleTypeDef hdma_adc;
extern TIM_HandleTypeDef htim6;
volatile bool updateAdcVal = false;

uint32_t adc_val[3];

void setup(){
	HAL_ADC_Start_DMA(&hadc, adc_val, 3);
	HAL_TIM_Base_Start(&htim6);
}

void HAL_ADC_ConvCpltCallback(ADC_HandleTypeDef *hadc) {
	updateAdcVal = true;
	HAL_GPIO_TogglePin(LED4_GPIO_Port, LED4_Pin);
}

void appLoop() {

	LED1(adc_val[0] < 512);
	LED2(adc_val[1] < 512);
	LED3(adc_val[2] < 512);

}
