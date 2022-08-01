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

#define OFF 0
#define ON 1
#define ON_NONE 2
#define OVER_HEAT 3
#define AUTO_ADJ 4
#define SAFE_LOCK 5

#define B_TEMP 20

#define CDS 0
#define VR 1

#define CDS_LIMIT 3000

#define SW1 ((SW_ON_GPIO_Port->IDR) & SW_ON_Pin)
#define SW2 ((SW_LOCK_GPIO_Port->IDR) & SW_LOCK_Pin)
#define SW3 ((SW_AUTO_GPIO_Port->IDR) & SW_AUTO_Pin)
#define SW4 ((SW_A_GPIO_Port->IDR) & SW_A_Pin)
#define SW5 ((SW_B_GPIO_Port->IDR) & SW_B_Pin)

#define BUZZ_ON BUZZ_GPIO_Port->BSRR |= BUZZ_Pin;
#define BUZZ_OFF BUZZ_GPIO_Port->BRR |= BUZZ_Pin;

#define LED1_OFF LED1_GPIO_Port->BSRR |= LED1_Pin;
#define LED1_ON LED1_GPIO_Port->BRR |= LED1_Pin;
#define LED2_OFF LED2_GPIO_Port->BSRR |= LED2_Pin;
#define LED2_ON LED2_GPIO_Port->BRR |= LED2_Pin;
#define LED3_OFF LED3_GPIO_Port->BSRR |= LED3_Pin;
#define LED3_ON LED3_GPIO_Port->BRR |= LED3_Pin;
#define LED4_OFF LED4_GPIO_Port->BSRR |= LED4_Pin;
#define LED4_ON LED4_GPIO_Port->BRR |= LED4_Pin;
#define LED5_OFF LED5_GPIO_Port->BSRR |= LED5_Pin;
#define LED5_ON LED5_GPIO_Port->BRR |= LED5_Pin;

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
ADC_HandleTypeDef hadc;

/* USER CODE BEGIN PV */

int16_t vr;
uint8_t status=0, power=0, auto_adj=0, safe_lock=0, over_heat=0;
uint16_t temper=20, a_temper=20, auto_temper=220, temper_upspeed=900, temper_dnspeed=2900;
uint8_t i, fire=0, fire_ring=0, sw_flag=0, alarm_flag=0;

const char *status_fonts[6]={"OFF      ","ON       ","ON(NONE) ","OVER HEAT","AUTO ADJ ","SAFE LOCK"};

uint32_t r_count, up_count, dn_count, b_count, a_count;

char temp[40];
char temp2[40];
char temp3[40];

char lcd_buff[40];
uint32_t led_ring_data[12];


/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_ADC_Init(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
uint16_t read_adc(uint8_t x)
{
  uint16_t adc[2];

  HAL_ADC_Start(&hadc);
  HAL_ADC_PollForConversion(&hadc, 1000);
  adc[0]=HAL_ADC_GetValue(&hadc);
  HAL_ADC_PollForConversion(&hadc, 1000);
  adc[1]=HAL_ADC_GetValue(&hadc);
  HAL_ADC_Stop(&hadc);

  return adc[x];
}

void status_cal()
{
  if(over_heat) status=OVER_HEAT;
  else if(safe_lock) status=SAFE_LOCK;
  else if(power==0) status=OFF;
  else if(power && (read_adc(CDS)<CDS_LIMIT)) status=ON_NONE;
  else if(auto_adj && (read_adc(CDS)>=CDS_LIMIT)) status=AUTO_ADJ;
  else if(power && (read_adc(CDS)>=CDS_LIMIT)) status=ON;
}

void lcd_dis()
{
  sprintf(temp,"TEMP:%03d%cC  %c:%d ", temper, 0xDF, 1, fire);
  sprintf(temp2,"%03d",a_temper);
  lcd_gotoxy(1,1); lcd_puts(temp);
  lcd_gotoxy(1,2); lcd_puts("[");
  lcd_gotoxy(11,2); lcd_puts("][");
  lcd_gotoxy(16,2); lcd_puts("]");
  lcd_gotoxy(2,2); lcd_puts(status_fonts[status]);
  lcd_gotoxy(13,2); lcd_puts(temp2);
}

void led_ring()
{
  if((HAL_GetTick()-r_count)>=100)
  {
    if(fire!=fire_ring)
    {
      if(fire>fire_ring) fire_ring++;
      else fire_ring--;
    }

    if(fire_ring>0)
    {
      if(fire_ring<=3)
      {
        for(i=1; i<12; i+=2)
        {
          led_ring_data[i]=led_ring_rgb(0,0,0);
        }
        for(i=2; i<12; i+=4)
        {
          led_ring_data[i]=led_ring_rgb(0,0,0);
        }

        if(fire_ring==1) {led_ring_data[0]=led_ring_data[4]=led_ring_data[8]=led_ring_rgb(0,0,13);}
        else if(fire_ring==2) {led_ring_data[0]=led_ring_data[4]=led_ring_data[8]=led_ring_rgb(0,0,76);}
        else {led_ring_data[0]=led_ring_data[4]=led_ring_data[8]=led_ring_rgb(0,0,255);}
      }
      else {led_ring_data[0]=led_ring_data[4]=led_ring_data[8]=led_ring_rgb(0,0,255);}

      if(fire_ring>3 && fire_ring<=6)
      {
        for(i=1; i<12; i+=2)
        {
          led_ring_data[i]=led_ring_rgb(0,0,0);
        }

        if(fire_ring==4) {led_ring_data[2]=led_ring_data[6]=led_ring_data[10]=led_ring_rgb(0,0,13);}
        else if(fire_ring==5) {led_ring_data[2]=led_ring_data[6]=led_ring_data[10]=led_ring_rgb(0,0,76);}
        else {led_ring_data[2]=led_ring_data[6]=led_ring_data[10]=led_ring_rgb(0,0,255);}
      }

      if(fire_ring>6 && fire_ring<=9)
      {
        if(fire_ring==7) {led_ring_data[1]=led_ring_data[3]=led_ring_data[5]=led_ring_data[7]=led_ring_data[9]=led_ring_data[11]=led_ring_rgb(0,0,13);}
        else if(fire_ring==8) {led_ring_data[1]=led_ring_data[3]=led_ring_data[5]=led_ring_data[7]=led_ring_data[9]=led_ring_data[11]=led_ring_rgb(0,0,76);}
        else {led_ring_data[1]=led_ring_data[3]=led_ring_data[5]=led_ring_data[7]=led_ring_data[9]=led_ring_data[11]=led_ring_rgb(0,0,255);}
      }
    }
    else
    {
      for(i=0; i<12; i++)
      {
        led_ring_data[i]=led_ring_rgb(0,0,0);
      }
    }

    led_ring_update(led_ring_data);
    r_count=HAL_GetTick();
  }
}

void temper_updown()
{
  if((temper-B_TEMP)<10) temper_dnspeed=2900;
  else if((temper-B_TEMP)>=300) temper_dnspeed=100;
  else if((temper-B_TEMP)>=200) temper_dnspeed=200;
  else if((temper-B_TEMP)>=100) temper_dnspeed=400;
  else if((temper-B_TEMP)>=40) temper_dnspeed=700;
  else if((temper-B_TEMP)>=20) temper_dnspeed=1100;
  else if((temper-B_TEMP)>=15) temper_dnspeed=1600;
  else if((temper-B_TEMP)>=10) temper_dnspeed=2200;
  else dn_count=HAL_GetTick();

  if(fire==0) up_count=HAL_GetTick();
  else
  {
    temper_upspeed=(1000-(fire*100));
  }

  if(HAL_GetTick()-dn_count>=temper_dnspeed)
  {
    temper--;
    dn_count=HAL_GetTick();
  }

  if(HAL_GetTick()-up_count>=temper_upspeed)
  {
    temper++;
    up_count=HAL_GetTick();
  }

  if(temper>999) temper=999;
  else if(temper<20) temper=20;

}

void buzz_3sound(uint8_t *b_flag)
{
  if(*b_flag)
  {
    if(HAL_GetTick()-b_count<=200) BUZZ_ON
        else if(HAL_GetTick()-b_count<=400) BUZZ_OFF
        else if(HAL_GetTick()-b_count<=600) BUZZ_ON
        else if(HAL_GetTick()-b_count<=800) BUZZ_OFF
        else if(HAL_GetTick()-b_count<=1000) BUZZ_ON
        else if(HAL_GetTick()-b_count<=1200) BUZZ_OFF
        else
        {
          BUZZ_OFF
          b_count=HAL_GetTick();
          *b_flag=0;
        }
  }
}

void led_con(uint8_t led)
{
  if(led==1)
  {
    LED1_ON
    LED2_OFF
    LED3_OFF
    LED4_OFF
    LED5_OFF
  }
  else if(led==2)
  {
    LED1_OFF
    LED2_ON
    LED3_OFF
    LED4_OFF
    LED5_OFF
  }
  else if(led==3)
  {
    LED1_OFF
    LED2_OFF
    LED3_ON
    LED4_OFF
    LED5_OFF
  }
  else if(led==4)
  {
    LED1_OFF
    LED2_OFF
    LED3_OFF
    LED4_ON
    LED5_OFF
  }
  else if(led==5)
  {
    LED1_OFF
    LED2_OFF
    LED3_OFF
    LED4_OFF
    LED5_ON
  }
  else
  {
    LED1_OFF
    LED2_OFF
    LED3_OFF
    LED4_OFF
    LED5_OFF
  }
}

void auto_adj_mode()
{
  vr = read_adc(VR);

  if(vr<=600) {auto_temper=80; led_con(1);}
  else if(vr<=1600) {auto_temper=100; led_con(2);}
  else if(vr<=2600) {auto_temper=140; led_con(3);}
  else if(vr<=3500) {auto_temper=180; led_con(4);}
  else {auto_temper=220; led_con(5);}

  if(auto_temper-temper>1) fire=9;
  else if(auto_temper-temper<-1) fire=1;
}

void alarm_set()
{
  if(!SW4 || !SW5)
  {
    if(!sw_flag)
    {
      if(!SW4) a_temper-=20;
      else a_temper+=20;

      if(a_temper>280) a_temper=280;
      else if(a_temper<20) a_temper=20;

      sw_flag=1;
    }
  }
  else sw_flag=0;
}

void alarm_buzz()
{
  if((a_temper!=20) && (a_temper<=temper))
  {
    if(alarm_flag==0)
    {
      alarm_flag=1;
      a_count=HAL_GetTick();
    }

    if(HAL_GetTick()-a_count<=100) BUZZ_ON
        else if(HAL_GetTick()-a_count<=200) BUZZ_OFF
        else if(HAL_GetTick()-a_count<=300) BUZZ_ON
        else if(HAL_GetTick()-a_count<=1000) BUZZ_OFF
        else
        {
          BUZZ_OFF
          a_count=HAL_GetTick();
        }
  }
  else
  {
    BUZZ_OFF
    alarm_flag=0;
  }
}

/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{
  /* USER CODE BEGIN 1 */

  uint8_t b_flag=0, sw3_flag=0;

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

  lcd_cgram(1,0);

  led_ring_update(led_ring_data);

  lcd_puts("\fSmart Gas Range\n              01");
  HAL_Delay(2000);

  r_count = up_count = dn_count = HAL_GetTick();

  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */

    if(SW1==0) power=1;
    else power=0;

    if(temper>300)
    {
      over_heat=1;
      b_flag=1;
      b_count=HAL_GetTick();
    }

    if(!SW2) safe_lock=1;

    if(!SW3)
    {
      if(!sw3_flag)
      {
        if(!auto_adj) auto_adj=1;
        else auto_adj=0;
      }
      sw3_flag=1;
    }
    else sw3_flag=0;

    status_cal();

    alarm_set();

    if(power && status!=OVER_HEAT && status!=SAFE_LOCK) alarm_buzz();

    if(status==OFF) fire=0;
    else if(status==ON_NONE) fire=1;

    if(status==OVER_HEAT)
    {
      fire=0;
      buzz_3sound(&b_flag);
      if((!power) && temper<150) over_heat=0;
    }

    else if(status==SAFE_LOCK)
    {
      fire=0;
      if(SW2 && SW1) safe_lock=0;
    }

    if(status==ON)
    {
      vr = read_adc(VR);
      vr-=65;
      if(vr<0) vr=0;
      vr/=503.75;
      vr+=1;

      fire=vr;
    }

    if(status==AUTO_ADJ)
    {
      auto_adj_mode();
    }
    else led_con(0);

    led_ring();

    temper_updown();

    lcd_dis();

  }
  /* USER CODE END 3 */
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

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
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }
  /** Initializes the CPU, AHB and APB busses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_1) != HAL_OK)
  {
    Error_Handler();
  }
}

/**
  * @brief ADC Initialization Function
  * @param None
  * @retval None
  */
static void MX_ADC_Init(void)
{

  /* USER CODE BEGIN ADC_Init 0 */

  /* USER CODE END ADC_Init 0 */

  ADC_ChannelConfTypeDef sConfig = {0};

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
  if (HAL_ADC_Init(&hadc) != HAL_OK)
  {
    Error_Handler();
  }
  /** Configure for the selected ADC regular channel to be converted.
  */
  sConfig.Channel = ADC_CHANNEL_0;
  sConfig.Rank = ADC_RANK_CHANNEL_NUMBER;
  if (HAL_ADC_ConfigChannel(&hadc, &sConfig) != HAL_OK)
  {
    Error_Handler();
  }
  /** Configure for the selected ADC regular channel to be converted.
  */
  sConfig.Channel = ADC_CHANNEL_1;
  if (HAL_ADC_ConfigChannel(&hadc, &sConfig) != HAL_OK)
  {
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
static void MX_GPIO_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOC_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOA, LED_RING_Pin|BUZZ_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOA, LED1_Pin|LED2_Pin|LED3_Pin|LED4_Pin
                          |LED5_Pin, GPIO_PIN_SET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOB, LCD_RS_Pin|LCD_RW_Pin|LCD_EN_Pin|LCD_D4_Pin
                          |LCD_D5_Pin|LCD_D6_Pin|LCD_D7_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pins : SW_A_Pin SW_B_Pin */
  GPIO_InitStruct.Pin = SW_A_Pin|SW_B_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

  /*Configure GPIO pins : SW_ON_Pin SW_AUTO_Pin SW_LOCK_Pin */
  GPIO_InitStruct.Pin = SW_ON_Pin|SW_AUTO_Pin|SW_LOCK_Pin;
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
  GPIO_InitStruct.Pin = LED1_Pin|LED2_Pin|LED3_Pin|LED4_Pin
                          |LED5_Pin|BUZZ_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /*Configure GPIO pins : LCD_RS_Pin LCD_RW_Pin LCD_EN_Pin LCD_D4_Pin
                           LCD_D5_Pin LCD_D6_Pin LCD_D7_Pin */
  GPIO_InitStruct.Pin = LCD_RS_Pin|LCD_RW_Pin|LCD_EN_Pin|LCD_D4_Pin
                          |LCD_D5_Pin|LCD_D6_Pin|LCD_D7_Pin;
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
void Error_Handler(void)
{
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
