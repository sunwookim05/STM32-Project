#include "main.h"

#ifndef __ED_RING_H_
#define __ED_RING_H_

#define MAX_LED 12
#define USE_BRIGHTNESS 0

extern TIM_HandleTypeDef htim2;
extern DMA_HandleTypeDef hdma_tim2_ch1;

void Set_LED(int LEDnum, int Red, int Green, int Blue);
void WS2812_Send(void);
void Set_Brightness(int brightness);

#endif
