/* USER CODE BEGIN Header */
/**
 ******************************************************************************
 * @file           : main.c
 * @brief          : Main program body
 ******************************************************************************
 * @attention
 *
 * <h2><center>&copy; Copyright (c) 2020 STMicroelectronics.
 * All rights reserved.</center></h2>
 *
 * This software component is licensed by ST under BSD 3-Clause license,
 * the "License"; You may not use this file except in compliance with the
 * License. You may obtain a copy of the License at:
 *                        opensource.org/licenses/BSD-3-Clause
 *
 ******************************************************************************
 */
/* USER CODE END Header */

/* Includes ------------------------------------------------------------------*/
#include "main.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include <stdio.h>
#include "lcd1602.h"
#include "control_hardware.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
#define SW1 (HAL_GPIO_ReadPin(SW_ON_GPIO_Port, SW_ON_Pin))
#define SW2 (HAL_GPIO_ReadPin(SW_LOCK_GPIO_Port, SW_LOCK_Pin))
#define SW3 (HAL_GPIO_ReadPin(SW_AUTO_GPIO_Port, SW_AUTO_Pin))
#define SW4 (HAL_GPIO_ReadPin(SW_A_GPIO_Port, SW_A_Pin))
#define SW5 (HAL_GPIO_ReadPin(SW_B_GPIO_Port, SW_B_Pin))

#define TEMPUP(X) (X == 1 ? 900 : X == 2 ? 800 : X == 3 ? 700 : X == 4 ? 600 : X == 5 ? 500 : X == 6 ? 400 : X == 7 ? 300 : X == 8 ? 200 : X == 9 ? 100 : 0)
#define TEMPDOWN(X) (X < 10 ? 2900 : X >= 300 ? 100 : X >= 200 ? 200 : X >= 100 ? 400 : X >= 40 ? 700 : X >= 20 ? 1100 : X >= 15 ? 1600 : X >= 10 ? 2200 : 0)

#define BUZZER(X) (HAL_GPIO_WritePin(BUZZ_GPIO_Port, BUZZ_Pin, X))
#define LED(N, X) (HAL_GPIO_WritePin(LED##N##_GPIO_Port, LED##N##_Pin, !X))

#define NOW HAL_GetTick()

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */
typedef char* String;
/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
ADC_HandleTypeDef hadc;

/* USER CODE BEGIN PV */
int temp = 20;
int power = 0;
int powerSet = 0;
int uNum = 0;
int altemp = 20;
int gasflag = 0;
int safeflag = 0;
int highflag = 0;
int autoFlag = 0;
int asetTemp;
char bf[17];
String uistat[6] = { "OFF      ", "ON       ", "ON(NONE) ", "OVER HEAT",
		"AUTO ADJ ", "SAFE LOCK" };
uint32_t led_ring_data[10][12] = { 0 };

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_ADC_Init(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
void adc(uint16_t *vr, uint16_t *cds) {
	HAL_ADC_Start(&hadc);
	HAL_ADC_PollForConversion(&hadc, 1000);
	*cds = HAL_ADC_GetValue(&hadc);
	HAL_ADC_PollForConversion(&hadc, 1000);
	*vr = HAL_ADC_GetValue(&hadc);
	HAL_ADC_Stop(&hadc);
}

void ledRing() {
	int i;
	for (i = 0; i < 12; i++)
		led_ring_data[0][i] = led_ring_rgb(0, 0, 0);
	for (i = 0; i < 12; i++)
		if (i == 0 || i == 4 || i == 8)
			led_ring_data[1][i] = led_ring_rgb(0, 0, 13);
		else
			led_ring_data[1][i] = led_ring_rgb(0, 0, 0);
	for (i = 0; i < 12; i++)
		if (i == 0 || i == 4 || i == 8)
			led_ring_data[2][i] = led_ring_rgb(0, 0, 76);
		else
			led_ring_data[2][i] = led_ring_rgb(0, 0, 0);
	for (i = 0; i < 12; i++)
		if (i == 0 || i == 4 || i == 8)
			led_ring_data[3][i] = led_ring_rgb(0, 0, 255);
		else
			led_ring_data[3][i] = led_ring_rgb(0, 0, 0);
	for (i = 0; i < 12; i++)
		if (i == 0 || i == 4 || i == 8)
			led_ring_data[4][i] = led_ring_rgb(0, 0, 255);
		else if (i == 2 || i == 6 || i == 10)
			led_ring_data[4][i] = led_ring_rgb(0, 0, 13);
		else
			led_ring_data[4][i] = led_ring_rgb(0, 0, 0);
	for (i = 0; i < 12; i++)
		if (i == 0 || i == 4 || i == 8)
			led_ring_data[5][i] = led_ring_rgb(0, 0, 255);
		else if (i == 2 || i == 6 || i == 10)
			led_ring_data[5][i] = led_ring_rgb(0, 0, 76);
		else
			led_ring_data[5][i] = led_ring_rgb(0, 0, 0);
	for (i = 0; i < 12; i++)
		if (i == 0 || i == 2 || i == 4 || i == 6 || i == 8 || i == 10)
			led_ring_data[6][i] = led_ring_rgb(0, 0, 255);
		else
			led_ring_data[6][i] = led_ring_rgb(0, 0, 0);
	for (i = 0; i < 12; i++)
		if (i == 0 || i == 2 || i == 4 || i == 6 || i == 8 || i == 10)
			led_ring_data[7][i] = led_ring_rgb(0, 0, 255);
		else
			led_ring_data[7][i] = led_ring_rgb(0, 0, 13);
	for (i = 0; i < 12; i++)
		if (i == 0 || i == 2 || i == 4 || i == 6 || i == 8 || i == 10)
			led_ring_data[8][i] = led_ring_rgb(0, 0, 255);
		else
			led_ring_data[8][i] = led_ring_rgb(0, 0, 76);
	for (i = 0; i < 12; i++)
		led_ring_data[9][i] = led_ring_rgb(0, 0, 255);
}

void lcd_print() {
	sprintf(bf, "TEMP:%03d%cC  %c:%d", temp, 0xDF, 1, power);
	lcd_gotoxy(0, 1);
	lcd_puts(bf);
	sprintf(bf, "[%.9s][%03d]", uistat[uNum], altemp);
	lcd_gotoxy(0, 0);
	lcd_puts(bf);
}

void gasOn(uint16_t vr, uint16_t cds) {
	if (cds > 3000) {
		uNum = 1;
		powerSet = vr / 511.875 + 1;
	} else {
		uNum = 2;
		powerSet = 1;
	}
}

void ledClear() {
	LED(1, 0);
	LED(2, 0);
	LED(3, 0);
	LED(4, 0);
	LED(5, 0);
}

void ledControl(uint16_t x) {
	ledClear();
	if (x == 1) {
		LED(1, 1);
		asetTemp = 80;
	} else if (x == 2) {
		LED(2, 1);
		asetTemp = 100;
	} else if (x == 3) {
		LED(3, 1);
		asetTemp = 140;
	} else if (x == 4) {
		LED(4, 1);
		asetTemp = 180;
	} else if (x == 5) {
		LED(5, 1);
		asetTemp = 220;
	}
}

void autoOn(uint16_t vr, uint16_t cds) {
	int i = asetTemp - temp;
	if (cds > 3000) {
		ledControl((vr / 1023.75 + 1));
		uNum = 4;
		if (i < -1)
			powerSet = 1;
		if (1 < i)
			powerSet = 9;
	} else {
		uNum = 2;
		powerSet = 1;
	}
}

void safemode() {
	uNum = 5;
	powerSet = 0;
}
/* USER CODE END 0 */

/**
 * @brief  The application entry point.
 * @retval int
 */
int main(void) {
	/* USER CODE BEGIN 1 */
	int buzflag = 0, swFlag = 0, alflag = 0;
	uint16_t vr, cds;
	uint32_t last = NOW;
	uint32_t plast = NOW;
	uint32_t tdownlast = NOW;
	uint32_t tuplast = NOW;
	uint32_t buzlast;
	uint32_t allast;
	/* USER CODE END 1 */

	/* MCU Configuration--------------------------------------------------------*/

	/* Reset of all peripherals, Initializes the Flash interface and the Systick. */
	HAL_Init();

	/* USER CODE BEGIN Init */

	/* USER CODE END Init */

	/* Configure the system clock */
	SystemClock_Config();

	/* USER CODE BEGIN SysInit */

	/* USER CODE END SysInit */

	/* Initialize all configured peripherals */
	MX_GPIO_Init();
	MX_ADC_Init();
	/* USER CODE BEGIN 2 */
	LcdInit();
	ledRing();
	lcd_cgram(1, 0);
	lcd_puts("\fSmart Gas Range\n              01");
	HAL_Delay(2000);
	lcd_print();
	ledRing();

	/* USER CODE END 2 */

	/* Infinite loop */
	/* USER CODE BEGIN WHILE */
	while (1) {
		/* USER CODE END WHILE */
		/* USER CODE BEGIN 3 */
		if (gasflag) {
			if (safeflag)
				safemode();
			else if (autoFlag)
				autoOn(vr, cds);
			else
				gasOn(vr, cds);
		} else {
			uNum = 0;
			powerSet = 0;
		}
		if (highflag) {
			uNum = 3;
			powerSet = 0;
			if (!buzflag) {
				buzlast = NOW;
				buzflag = 1;
			}
			if ((NOW - buzlast >= 100 && NOW - buzlast <= 200)
					|| (NOW - buzlast >= 300 && NOW - buzlast <= 400)
					|| (NOW - buzlast >= 500 && NOW - buzlast <= 600)) {
				BUZZER(1);
			} else {
				BUZZER(0);
			}
			if (temp < 150 && !SW1) {
				highflag = 0;
				buzflag = 0;
			}
		}
		if (!SW3 || !SW4 || !SW5) {
			if (!SW3 && !swFlag) {
				if (!autoFlag) {
					autoFlag = 1;
				} else {
					autoFlag = 0;
					ledClear();
				}
			}
			if (!SW4 && !swFlag)
				altemp -= 20;
			if (!SW5 && !swFlag)
				altemp += 20;
			if (altemp > 280)
				altemp = 280;
			if (altemp < 20)
				altemp = 20;
			swFlag = 1;
		} else {
			swFlag = 0;
		}
		if (altemp < temp && altemp > 20) {
			if (!alflag) {
				allast = NOW;
				alflag = 1;
			}
		} else {
			BUZZER(0);
			alflag = 0;
		}
		if (alflag) {
			if ((NOW - allast >= 0 && NOW - allast <= 100)
					|| (NOW - allast >= 200 && NOW - allast <= 300))
				BUZZER(1);
			else
				BUZZER(0);
			if (NOW - allast >= 1000)
				allast = NOW;
		}
		if (SW1 && !safeflag)
			gasflag = 1;
		if (!SW1 && !safeflag)
			gasflag = 0;
		if (SW2)
			safeflag = 1;
		if(!SW2 && SW1)
			safeflag = 0;
		if (NOW - last >= 10) {
			adc(&vr, &cds);
			if (NOW - plast >= 100) {
				if (power < powerSet)
					power++;
				if (power > powerSet)
					power--;
				led_ring_update(led_ring_data[power]);
				plast = NOW;
			}
			lcd_print();
			last = NOW;
		}
		if (NOW - tuplast >= TEMPUP(power) && TEMPUP(power) != 0) {
			temp++;
			if (temp > 300)
				highflag = 1;
			tuplast = NOW;
			lcd_print();
		}
		if (NOW - tdownlast >= TEMPDOWN(temp - 20) && TEMPDOWN(temp - 20) != 0) {
			temp--;
			if (temp < 20)
				temp = 20;
			tdownlast = NOW;
			lcd_print();
		}
	}
	/* USER CODE END 3 */
}

/**
 * @brief System Clock Configuration
 * @retval None
 */
void SystemClock_Config(void) {
	RCC_OscInitTypeDef RCC_OscInitStruct = { 0 };
	RCC_ClkInitTypeDef RCC_ClkInitStruct = { 0 };

	/** Configure the main internal regulator output voltage
	 */
	__HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);
	/** Initializes the CPU, AHB and APB busses clocks
	 */
	RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
	RCC_OscInitStruct.HSIState = RCC_HSI_ON;
	RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
	RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
	RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI;
	RCC_OscInitStruct.PLL.PLLMUL = RCC_PLLMUL_4;
	RCC_OscInitStruct.PLL.PLLDIV = RCC_PLLDIV_2;
	if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK) {
		Error_Handler();
	}
	/** Initializes the CPU, AHB and APB busses clocks
	 */
	RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_SYSCLK
			| RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2;
	RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
	RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
	RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
	RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

	if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_1) != HAL_OK) {
		Error_Handler();
	}
}

/**
 * @brief ADC Initialization Function
 * @param None
 * @retval None
 */
static void MX_ADC_Init(void) {

	/* USER CODE BEGIN ADC_Init 0 */

	/* USER CODE END ADC_Init 0 */

	ADC_ChannelConfTypeDef sConfig = { 0 };

	/* USER CODE BEGIN ADC_Init 1 */

	/* USER CODE END ADC_Init 1 */
	/** Configure the global features of the ADC (Clock, Resolution, Data Alignment and number of conversion)
	 */
	hadc.Instance = ADC1;
	hadc.Init.OversamplingMode = DISABLE;
	hadc.Init.ClockPrescaler = ADC_CLOCK_SYNC_PCLK_DIV2;
	hadc.Init.Resolution = ADC_RESOLUTION_12B;
	hadc.Init.SamplingTime = ADC_SAMPLETIME_160CYCLES_5;
	hadc.Init.ScanConvMode = ADC_SCAN_DIRECTION_FORWARD;
	hadc.Init.DataAlign = ADC_DATAALIGN_RIGHT;
	hadc.Init.ContinuousConvMode = DISABLE;
	hadc.Init.DiscontinuousConvMode = DISABLE;
	hadc.Init.ExternalTrigConvEdge = ADC_EXTERNALTRIGCONVEDGE_NONE;
	hadc.Init.ExternalTrigConv = ADC_SOFTWARE_START;
	hadc.Init.DMAContinuousRequests = DISABLE;
	hadc.Init.EOCSelection = ADC_EOC_SINGLE_CONV;
	hadc.Init.Overrun = ADC_OVR_DATA_PRESERVED;
	hadc.Init.LowPowerAutoWait = DISABLE;
	hadc.Init.LowPowerFrequencyMode = DISABLE;
	hadc.Init.LowPowerAutoPowerOff = DISABLE;
	if (HAL_ADC_Init(&hadc) != HAL_OK) {
		Error_Handler();
	}
	/** Configure for the selected ADC regular channel to be converted.
	 */
	sConfig.Channel = ADC_CHANNEL_0;
	sConfig.Rank = ADC_RANK_CHANNEL_NUMBER;
	if (HAL_ADC_ConfigChannel(&hadc, &sConfig) != HAL_OK) {
		Error_Handler();
	}
	/** Configure for the selected ADC regular channel to be converted.
	 */
	sConfig.Channel = ADC_CHANNEL_1;
	if (HAL_ADC_ConfigChannel(&hadc, &sConfig) != HAL_OK) {
		Error_Handler();
	}
	/* USER CODE BEGIN ADC_Init 2 */

	/* USER CODE END ADC_Init 2 */

}

/**
 * @brief GPIO Initialization Function
 * @param None
 * @retval None
 */
static void MX_GPIO_Init(void) {
	GPIO_InitTypeDef GPIO_InitStruct = { 0 };

	/* GPIO Ports Clock Enable */
	__HAL_RCC_GPIOC_CLK_ENABLE();
	__HAL_RCC_GPIOA_CLK_ENABLE();
	__HAL_RCC_GPIOB_CLK_ENABLE();

	/*Configure GPIO pin Output Level */
	HAL_GPIO_WritePin(GPIOA, LED_RING_Pin | BUZZ_Pin, GPIO_PIN_RESET);

	/*Configure GPIO pin Output Level */
	HAL_GPIO_WritePin(GPIOA,
	LED1_Pin | LED2_Pin | LED3_Pin | LED4_Pin | LED5_Pin, GPIO_PIN_SET);

	/*Configure GPIO pin Output Level */
	HAL_GPIO_WritePin(GPIOB,
			LCD_RS_Pin | LCD_RW_Pin | LCD_EN_Pin | LCD_D4_Pin | LCD_D5_Pin
					| LCD_D6_Pin | LCD_D7_Pin, GPIO_PIN_RESET);

	/*Configure GPIO pins : SW_A_Pin SW_B_Pin */
	GPIO_InitStruct.Pin = SW_A_Pin | SW_B_Pin;
	GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
	GPIO_InitStruct.Pull = GPIO_PULLUP;
	HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

	/*Configure GPIO pins : SW_ON_Pin SW_AUTO_Pin SW_LOCK_Pin */
	GPIO_InitStruct.Pin = SW_ON_Pin | SW_AUTO_Pin | SW_LOCK_Pin;
	GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
	GPIO_InitStruct.Pull = GPIO_PULLUP;
	HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

	/*Configure GPIO pin : LED_RING_Pin */
	GPIO_InitStruct.Pin = LED_RING_Pin;
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_OD;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
	HAL_GPIO_Init(LED_RING_GPIO_Port, &GPIO_InitStruct);

	/*Configure GPIO pins : LED1_Pin LED2_Pin LED3_Pin LED4_Pin
	 LED5_Pin BUZZ_Pin */
	GPIO_InitStruct.Pin = LED1_Pin | LED2_Pin | LED3_Pin | LED4_Pin | LED5_Pin
			| BUZZ_Pin;
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
	HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

	/*Configure GPIO pins : LCD_RS_Pin LCD_RW_Pin LCD_EN_Pin LCD_D4_Pin
	 LCD_D5_Pin LCD_D6_Pin LCD_D7_Pin */
	GPIO_InitStruct.Pin = LCD_RS_Pin | LCD_RW_Pin | LCD_EN_Pin | LCD_D4_Pin
			| LCD_D5_Pin | LCD_D6_Pin | LCD_D7_Pin;
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
	HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

}

/* USER CODE BEGIN 4 */

/* USER CODE END 4 */

/**
 * @brief  This function is executed in case of error occurrence.
 * @retval None
 */
void Error_Handler(void) {
	/* USER CODE BEGIN Error_Handler_Debug */
	/* User can add his own implementation to report the HAL error return state */

	/* USER CODE END Error_Handler_Debug */
}

#ifdef  USE_FULL_ASSERT
/**
 * @brief  Reports the name of the source file and the source line number
 *         where the assert_param error has occurred.
 * @param  file: pointer to the source file name
 * @param  line: assert_param error line source number
 * @retval None
 */
void assert_failed(uint8_t *file, uint32_t line)
{ 
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     tex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
