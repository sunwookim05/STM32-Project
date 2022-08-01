#include "main.h"
#include "App.h"

#define LED1(x) (HAL_GPIO_WritePin(LED1_GPIO_Port, LED1_Pin, x))
#define LED2(x) (HAL_GPIO_WritePin(LED2_GPIO_Port, LED2_Pin, x))
#define SW1 (HAL_GPIO_ReadPin(S1_GPIO_Port, S1_Pin))
#define NOW (HAL_GetTick())

enum {
	SAMPLE_SIG
};

void appLoop() {

#define BSP_TICKS_PER_SEC 1000

	static enum {
		INITIAL, OFF_STATE, ON_STATE
	} state = INITIAL;
	static uint32_t btnDebounceCounter = 0;
	static uint32_t start;
	static uint8_t prev_button = 1u;

	/* buffering inputs*/
	struct {
		uint16_t sig;
		/* event parameters */
		uint32_t now;
		uint8_t button;
	} evt, *e;
	evt.sig = SAMPLE_SIG;
	evt.now = HAL_GetTick();
	evt.button = HAL_GPIO_ReadPin(S1_GPIO_Port, S1_Pin);
	e = &evt;

	switch (state) {
	case INITIAL:
		start = e->now;
		state = OFF_STATE;
		HAL_GPIO_WritePin(LED2_GPIO_Port, LED2_Pin, 1U);
		break;
	case OFF_STATE:
		if (e->now - start > BSP_TICKS_PER_SEC * 1U / 4U) {
			start = e->now;
			state = ON_STATE;
			HAL_GPIO_WritePin(LED1_GPIO_Port, LED1_Pin, 1);
		}
		break;
	case ON_STATE:
		if (e->now - start > BSP_TICKS_PER_SEC * 3U / 4U) {
			start = e->now;
			state = OFF_STATE;
			HAL_GPIO_WritePin(LED1_GPIO_Port, LED1_Pin, 0);
		}
		break;
	default:
		break;
	}

	if (e->now - btnDebounceCounter > 10) {
		if (prev_button != 0U && e->button == 0U) { /* falling edge */
			HAL_GPIO_TogglePin(LED2_GPIO_Port, LED2_Pin);
			//HAL_GPIO_WritePin(LED2_GPIO_Port, LED2_Pin, 1U);
		}
		prev_button = e->button;
		btnDebounceCounter = e->now;
		//	else if (prev_button == 0U && e->button != 0U) {/* rising edge */
		//		HAL_GPIO_WritePin(LED2_GPIO_Port, LED2_Pin, 0U);
		//		prev_button = e->button;
		//	}
	}

//	static uint8_t btn_last;
//	static uint32_t last_t;
//
//	if(NOW - last_t > 25){
//		if(btn_last == 1 && SW1 == 0)HAL_GPIO_TogglePin(LED1_GPIO_Port, LED1_Pin);
//		else if(btn_last == 0 && SW1 == 1)HAL_GPIO_TogglePin(LED2_GPIO_Port, LED2_Pin);
////		btn_last == 1 && SW1 == 0 ? HAL_GPIO_TogglePin(LED1_GPIO_Port, LED1_Pin): NULL;
////		btn_last == 0 && SW1 == 1 ? HAL_GPIO_TogglePin(LED2_GPIO_Port, LED2_Pin): NULL;
//
//		btn_last = SW1;
//		last_t = NOW;
//	}

//	if(SW1 == 0){
//		while(SW1 == 0) {}
//		HAL_GPIO_TogglePin(LED1_GPIO_Port, LED1_Pin);
//	}

}
