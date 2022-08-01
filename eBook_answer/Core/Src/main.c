/* USER CODE BEGIN Header */
/**
 ******************************************************************************
 * @file           : main.c
 * @brief          : Main program body
 ******************************************************************************
 * @attention
 *
 * <h2><center>&copy; Copyright (c) 2022 STMicroelectronics.
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
#include "fatfs.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "stdio.h"
#include "string.h"
#include "math.h"
#include "ssd1306.h"
#include "fonts.h"
#include "MPU6050.h"
#include "cap1203.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */
/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
#define SW1A (!(EN_A_GPIO_Port->IDR & EN_A_Pin))
#define SW1B (!(EN_B_GPIO_Port->IDR & EN_B_Pin))
#define SW1C !(EN_SW_GPIO_Port->IDR & EN_SW_Pin)
#define SW2 !(SW2_GPIO_Port->IDR & SW2_Pin)
#define BUZ(x) (HAL_GPIO_WritePin(GPIOB, GPIO_PIN_4, (!x)))
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
ADC_HandleTypeDef hadc;
DMA_HandleTypeDef hdma_adc;

I2C_HandleTypeDef hi2c1;

SPI_HandleTypeDef hspi1;

TIM_HandleTypeDef htim2;

/* USER CODE BEGIN PV */
char bf[50];
uint32_t value;
volatile float temp;
FATFS fs;
DIR dir;
mpu6050 Mpu6050;
typedef enum {
	basicMode = 0, bookMode, settingMode, readMode,
} _mode;
typedef enum {
	books = 0, setting, logMenu,
} _Item;
typedef enum {
	reading = 0, delete, mark,
} _Books;
typedef enum {
	rotate = 0, touch, bright, noSetting, brightAuto, brightManual,
} _Setting;
typedef enum {
	_SW1 = 0, _SW2, _SW_MAX,
} _SW;
typedef enum {
	cs0On = 1, cs1On, cs2On, rightSweep, leftSweep,
} _slideSW;
uint8_t firF, rotarySw, bookCount, nowBook, autoBright, angle, befoAngle,
		autoRotate, nowAngle, buzM;
uint32_t cnt, befoSw1c, nowSw1c, befoSw2, nowSw2, befoCap1203, nowCap1203,
		befoAngleC, nowAngleC, buzC;
_mode key = basicMode;

typedef struct BOOK {
	FSIZE_t bookSize;
	uint8_t bookMark;
	uint64_t page;
	uint64_t pageList[10];
	char bookName[40];
	char altName[13];

} _book;
_book bookArray[10];
char readingBook[340];
uint8_t bookLog[3] = { 255, 255, 255 };

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_DMA_Init(void);
static void MX_I2C1_Init(void);
static void MX_SPI1_Init(void);
static void MX_ADC_Init(void);
static void MX_TIM2_Init(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
void sort(void) {
	_book temp;
	for (uint8_t i = bookCount - 1; i > 0; i--) {
		for (uint8_t j = 0; j < i; j++) {
			if (0 < strcmp(bookArray[j].bookName, bookArray[j + 1].bookName)) {
				temp = bookArray[j];
				bookArray[j] = bookArray[j + 1];
				bookArray[j + 1] = temp;
			}
		}
	}
	for (uint8_t i = bookCount - 1; i > 0; i--) {
		for (uint8_t j = 0; j < i; j++) {
			if (bookArray[j].bookMark < bookArray[j + 1].bookMark) {
				temp = bookArray[j];
				bookArray[j] = bookArray[j + 1];
				bookArray[j + 1] = temp;
			}
		}
	}
}

void SSD1306_putsXY(uint8_t x, uint8_t y, char *str, uint8_t color) {
	if (nowAngle == 0)
		SSD1306_GotoXY(x * 8, (127 - y * 6) - 6);
	else if (nowAngle == 1)
		SSD1306_GotoXY(y * 6, x * 8);
	else if (nowAngle == 2)
		SSD1306_GotoXY(127 - x * 8, y * 6);
	else
		SSD1306_GotoXY((126 - y * 6) - 6, 120 - x * 8);
	SSD1306_Puts(str, &Font_6x8, color, nowAngle);
}

void setBright(uint8_t temp) {
	uint8_t dt[3];
	dt[0] = 0;
	dt[1] = 0x81;
	dt[2] = temp;
	HAL_I2C_Master_Transmit(&hi2c1, SSD1306_I2C_ADDR_0, dt, 3, 10);
	HAL_I2C_Master_Transmit(&hi2c1, SSD1306_I2C_ADDR_1, dt, 3, 10);
}

void wLog(uint8_t num) {
	static uint8_t i = 0;
	if (bookLog[0] == num)
		return;
	else if (bookLog[1] == num) {
		bookLog[1] = bookLog[0];
		bookLog[0] = num;
		return;
	} else if (i > 2 || bookLog[2] == num) {
		bookLog[2] = bookLog[1];
		bookLog[1] = bookLog[0];
		bookLog[0] = num;
		return;
	}
	bookLog[i] = num;
	i++;
}

void _strtok(char *str, char *key) {
	uint8_t i = 0;
	while (1) {
		if (*(str + i) == *key) {
			*(str + i) = '\0';
			break;
		}

		else
			i++;
	}

}

void f_scanFiles(void) {
	volatile FRESULT res;
	FILINFO fno;
	res = f_mount(&fs, "", 0);
	res = f_opendir(&dir, "");
	res = f_readdir(&dir, &fno);
	while (1) {
		res = f_readdir(&dir, &fno);
		if (res != FR_OK || fno.fname[0] == 0)
			break;
		if (fno.fattrib && AM_DIR) {
			_strtok(fno.fname, ".");
			sprintf(bookArray[bookCount].bookName, "%s", fno.fname);
			sprintf(bookArray[bookCount].altName, "%s", fno.altname);
			bookArray[bookCount].bookSize = fno.fsize;
			bookCount++;
		}
	}
	f_closedir(&dir);
	res = f_mount(NULL, "", 0);
}

void f_openTxt(char *bookName, FSIZE_t ofs) {
	UINT nbrB = 0;
	FIL fil;
	volatile FRESULT res = FR_OK;
	char buf[13] = { 0, };
	strcpy(buf, bookName);
	res = f_mount(&fs, "", 1);
	if (res == FR_OK)
		res = f_open(&fil, buf, FA_READ);
	if (res == FR_OK)
		res = f_lseek(&fil, ofs);
	if (res == FR_OK)
		res = f_read(&fil, &readingBook, 340, &nbrB);
	if (res == FR_OK)
		res = f_close(&fil);
	res = f_mount(NULL, "", 1);
}

void strncopy(uint64_t *page, uint64_t *st, char *str) {
	int i = 0;
	for (i = 0; i < 10; i++) {
		str[i] = readingBook[(*st % 320) + i];
		if (str[i] == '\r') {
			str[i] = 0;
			i += 2;
			break;
		}
	}
	*page += i;
	*st += i;
}

void ro_str(char *str) {
	int len = strlen(str) - 1;
	for (int i = 0; i < len; i++) {
		char temp = str[len];
		for (int i = len; i > 0; i--)
			str[i] = str[i - 1];
		str[0] = temp;
	}
}

void _readRotation(uint8_t *count, uint8_t max) {

	if (rotarySw == 1) {
		if (*count < max)
			*count += 1;
	} else if (rotarySw == 2) {
		if (*count > 0)
			*count -= 1;
	}
	rotarySw = 0;

}
uint8_t read_sw(_SW sel) {
	const uint8_t now_sw[_SW_MAX] = { SW1C, SW2, };
	static uint32_t frev_tick[_SW_MAX] = { 0, };
	static uint8_t state[_SW_MAX] = { 0, };
	uint32_t now_tick = HAL_GetTick();

	if (now_sw[sel]) {
		if (now_tick - frev_tick[sel] > 50) {
			state[sel] = 1;
			frev_tick[sel] = now_tick;
		}
	} else {
		if (state[sel]) {
			state[sel] = 0;
			buzM = 1;
			return 1;
		}
	}
	return 0;
}

void getSlide(uint8_t *res) {
	uint8_t temp;
	static int cap1203State = 0;
	static _slideSW swipMode = 0;
	cap1203_getinputvalue(&temp);
	if (temp & 0x01)
		temp = cs0On;
	else if (temp & 0x02)
		temp = cs1On;
	else if (temp & 0x04)
		temp = cs2On;
	if (temp) {
		if (temp == cs0On) {
			if (!cap1203State && !swipMode) {
				cap1203State++;
				swipMode = rightSweep;
			} else if (cap1203State == 2 && swipMode == leftSweep)
				cap1203State++;
		} else if (temp == cs1On && nowCap1203 - befoCap1203 < 200) {
			if (cap1203State == 1 && swipMode)
				cap1203State++;
		} else if (temp == cs2On && nowCap1203 - befoCap1203 < 200) {
			if (!cap1203State && !swipMode) {
				cap1203State++;
				swipMode = leftSweep;
			} else if (cap1203State == 2 && swipMode == rightSweep)
				cap1203State++;
		}
		befoCap1203 = nowCap1203;
	}
	if (cap1203State == 3) {
		*res = swipMode;
		cap1203State = 0;
	} else
		*res = 0;
	if (nowCap1203 - befoCap1203 >= 200) {
		cap1203State = 0;
		swipMode = 0;
	}
}

void basicScreen() {
	static _Item sel = books;
	static uint8_t screenState = 0;
	if (!firF) {
		firF = 1;
		SSD1306_Fill(0);
		SSD1306_putsXY(1, 1, "-Ebook-", 0);
		SSD1306_putsXY(4, 0, "-Book", 0);
		SSD1306_putsXY(6, 0, "-Setting", 0);
		SSD1306_putsXY(8, 0, "-LOG", 0);
		if (sel == books && !screenState) {
			SSD1306_putsXY(4, 11, "-read", 0);
			SSD1306_putsXY(6, 11, "-delete", 0);
			SSD1306_putsXY(8, 11, "-mark", 0);
		}

		else if (sel == setting && !screenState) {
			SSD1306_putsXY(4, 11, "-rotate", 0);
			SSD1306_putsXY(6, 11, "-touch", 0);
			SSD1306_putsXY(8, 11, "-bright", 0);
		} else if (sel == logMenu || screenState) {
			for (int i = 0; i < 3; i++) {
				char str[13] = { 0, };
				strcpy(str,
						bookLog[i] != 255 ?
								bookArray[bookLog[i]].bookName : "empty");
				SSD1306_putsXY(4 + (i * 2), 11, str, 0);
			}
		}
		SSD1306_putsXY(4 + sel * 2, 9 + screenState, "<", 0);
		SSD1306_UpdateScreen();
	}
	angle = get_angle(&Mpu6050);
	firF = 0;
	_readRotation(&sel, 2);
	if (read_sw(_SW1)) {
		if (bookLog[sel] != 255 && screenState) {
			memset(readingBook, 0, sizeof(readingBook));
			HAL_Delay(200);
			f_openTxt(bookArray[bookLog[sel]].altName, 0);
			nowBook = bookLog[sel];
			key = readMode;
			bookArray[nowBook].page = 0;
			wLog(nowBook);
			screenState = 0;
		} else if (sel == logMenu)
			screenState = 10;
		else if (!screenState)
			key = sel + 1;
		sel = 0;
	}
	if (read_sw(_SW2)) {
		sel = 0;
		screenState = 0;
	}
}

void bookScreen(void) {
	static _Books sel = reading, modeState = reading, f_sel = reading;
	static uint8_t selMax = 2, screenState = 0, b_sel = 0;
	static uint32_t frev_tick = 0;
	static char _ro_str[40] = { 0, };
	if (!firF) {
		firF = 1;
		SSD1306_Fill(0);
		SSD1306_putsXY(1, 1, "-Ebook-", 0);
		SSD1306_putsXY(4, 0, "-read", 0);
		SSD1306_putsXY(6, 0, "-delete", 0);
		SSD1306_putsXY(8, 0, "-mark", 0);
		SSD1306_putsXY(1, 13, "-List-", 0);
		if (bookCount > 0)
			sort();
		for (int i = 0; i < bookCount; i++) {
			char str[13] = { 0, };
			char tmp[13] = { 0, };
			strcpy(str, (bookArray[i].bookMark ? "*" : ""));
			strncpy(tmp, bookArray[i].bookName, *str == '*' ? 9 : 10);
			strcat(str, tmp);
			if (i == sel && screenState) {
				if (!b_sel) {
					b_sel = 1;
					memset(_ro_str, 0, strlen(_ro_str));
					strcat(_ro_str, bookArray[i].bookName);
				}
				if (HAL_GetTick() - frev_tick >= 500) {
					frev_tick = HAL_GetTick();
					ro_str(_ro_str);
					SSD1306_putsXY(4 + (i * 2), 11, _ro_str, 0);
				}
			}
			SSD1306_putsXY(4 + (i * 2), 11,
					(i == sel && screenState) ? _ro_str : str, 0);
		}

		SSD1306_putsXY(4 + sel * 2, 9 + screenState, "<", 0);
		SSD1306_UpdateScreen();
	}
	angle = get_angle(&Mpu6050);
	firF = 0;
	_readRotation(&sel, selMax);
	if (sel != f_sel) {
		f_sel = sel;
		b_sel = 0;
	}
	if (read_sw(_SW1)) {
		if (screenState) {
			if (modeState == reading) {
				memset(readingBook, 0, sizeof(readingBook));
				f_openTxt(bookArray[sel].altName, 100);
				nowBook = sel;
				key = readMode;
				bookArray[nowBook].page = 0;
				wLog(nowBook);
			} else if (modeState == delete) {
				memset(&bookArray[sel], 0, sizeof(_book));
				for (int i = sel; i < bookCount; i++)
					bookArray[i] = bookArray[i + 1];
				bookCount--;
			} else if (modeState == mark) {
				bookArray[sel].bookMark = !bookArray[sel].bookMark;
			}
			screenState = 0;
			selMax = 2;
		} else {
			screenState = 11;
			selMax = bookCount - 1;
			modeState = sel;
		}
		sel = 0;
	}
	if (read_sw(_SW2)) {
		sel = 0;
		if (screenState) {
			selMax = 2;
			screenState = 0;
		} else {
			firF = 0;
			key = basicMode;
		}
	}
}

void readScreen(void) {
	_slideSW slideState = 0;
	static uint64_t st = 0;
	static FSIZE_t nowPage = 0;
	if (!firF) {
		HAL_Delay(100);
		st = 0;
		SSD1306_Fill(0);
		bookArray[nowBook].pageList[nowPage] = bookArray[nowBook].page;
		for (int i = 0; i < 30; i++) {
			char str[21] = { 0, };
			strncopy(&bookArray[nowBook].page, &st, str);
			SSD1306_putsXY(i % 15, i < 15 ? 0 : 11, str, 0);
		}
		sprintf(bf, "-%d-", nowPage * 2 + 1);
		SSD1306_putsXY(15, 3, bf, 0);
		sprintf(bf, "-%d-", nowPage * 2 + 2);
		SSD1306_putsXY(15, 14, bf, 0);
		SSD1306_UpdateScreen();
		firF = 1;
	}
	angle = get_angle(&Mpu6050);
	if (read_sw(_SW2)) {
		firF = 0;
		key = bookMode;
		nowPage = 0;
	}
	getSlide(&slideState);
	if (slideState) {
		if (slideState == rightSweep
				&& bookArray[nowBook].page < bookArray[nowBook].bookSize) {
			nowPage += 1;
		} else if (slideState == leftSweep) {
			nowPage -= nowPage > 0 ? 1 : 0;
			bookArray[nowBook].page = bookArray[nowBook].pageList[nowPage];
		}
		if (bookArray[nowBook].page < bookArray[nowBook].bookSize) {
			memset(readingBook, 0, sizeof(readingBook));
			f_openTxt(bookArray[nowBook].altName, bookArray[nowBook].page);
			firF = 0;
		}
	}
}

void settingScreen(void) {
	static _Setting sel = rotate, modeState = noSetting;
	static uint8_t screenState = 0, sensitivity = 1, setAutoBright = 0,
			brightness = 255, setAutoRotate = 0;
	static _slideSW slideState = 0;
	if (!firF) {
		firF = 1;
		SSD1306_Fill(0);
		SSD1306_putsXY(1, 0, "-Setting-", 0);
		SSD1306_putsXY(4, 0, "-rotate", 0);
		SSD1306_putsXY(6, 0, "-touch", 0);
		SSD1306_putsXY(8, 0, "-bright", 0);
		SSD1306_putsXY(
				4
						+ sel
								* ((modeState == bright
										|| modeState == brightManual) ? 5 : 2),
				9 + screenState, "<", 0);
		if (modeState == noSetting ? sel == rotate : modeState == rotate) {
			SSD1306_putsXY(1, 12, "-rotate-", 0);
			SSD1306_putsXY(4, 11, "-auto", 0);
			SSD1306_putsXY(5, 12, setAutoRotate ? "-ON " : "-OFF", 0);
		} else if (modeState == noSetting ? sel == touch : modeState == touch) {
			SSD1306_putsXY(1, 12, "-touch-", 0);
			SSD1306_putsXY(4, 11, "-set val", 0);
			sprintf(bf, "-%d", sensitivity);
			SSD1306_putsXY(5, 12, bf, 0);
		} else if (
				modeState == noSetting ?
						sel == bright :
						(modeState == bright || modeState == brightAuto
								|| modeState == brightManual)) {
			SSD1306_putsXY(1, 12, "-bright-", 0);
			SSD1306_putsXY(4, 11, "-auto", 0);
			SSD1306_putsXY(5, 12, setAutoBright ? "-ON " : "-OFF", 0);
			SSD1306_putsXY(9, 11, "-manual", 0);
			sprintf(bf, "-%03d", brightness);
			SSD1306_putsXY(10, 12, bf, 0);
		}

		SSD1306_UpdateScreen();
	}
	angle = get_angle(&Mpu6050);

	firF = 0;
	if (modeState == noSetting)
		_readRotation(&sel, 2);
	else if (modeState == touch)
		_readRotation(&sensitivity, 7);
	else if (modeState == bright)
		_readRotation(&sel, 1);
	else if (modeState == brightAuto)
		_readRotation(&setAutoBright, 1);
	else if (modeState == brightManual) {
		getSlide(&slideState);
		if (slideState == rightSweep)
			brightness = brightness + 10 < 255 ? brightness + 10 : 255;
		if (slideState == leftSweep)
			brightness = brightness - 10 > 0 ? brightness - 10 : 0;
		_readRotation(&brightness, 255);
	} else if (modeState == rotate)
		_readRotation(&setAutoRotate, 1);

	if (read_sw(_SW1)) {
		if (screenState) {
			if (modeState == touch)
				setSensitivity(7 - sensitivity);
			else if (modeState == bright)
				modeState = sel ? brightManual : brightAuto;
			else if (modeState == brightAuto) {
				autoBright = setAutoBright;
				modeState = bright;
			} else if (modeState == brightManual) {
				setBright(brightness);
				modeState = bright;
			}
		} else {
			modeState = sel;
			sel = 0;
			screenState = 10;
		}
	}
	if (read_sw(_SW2)) {
		if (screenState) {
			if (modeState == rotate)
				autoRotate = setAutoRotate;
			screenState = 0;
			modeState = noSetting;
		} else {
			firF = 0;
			key = basicMode;
		}

	}
}
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim) {
	cnt++;
	nowSw2++;
	nowSw1c++;
	nowCap1203++;
	nowAngleC++;
	if (autoBright) {
		temp = (float) (3000 * value / 4096) / 6.0;
		if (temp > 255)
			temp = 255;
		setBright(temp);
	}
	if (autoRotate) {
		if (angle == befoAngle)
			befoAngleC = nowAngleC;
		if ((nowAngleC - befoAngleC) > 250) {
			firF = 0;
			nowAngle = angle;
			befoAngle = nowAngle;
		}
	}
	if (buzM) {
		buzC++;
		if (buzC < 100)
			BUZ(1);
		else if (buzC < 150)
			BUZ(0);
		else if (buzC < 250)
			BUZ(1);
		else {
			buzC = buzM = 0;
			BUZ(0);
		}
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
  MX_DMA_Init();
  MX_I2C1_Init();
  MX_SPI1_Init();
  MX_ADC_Init();
  MX_TIM2_Init();
  MX_FATFS_Init();
  /* USER CODE BEGIN 2 */
	HAL_TIM_Base_Start_IT(&htim2);
	HAL_ADC_Start_DMA(&hadc, &value, 1);
	HAL_Delay(200);
	f_scanFiles();
	SSD1306_Init();
	SSD1306_ToggleInvert();
	init_MPU6050();
	cap1203_init(SENSITIVITY_2X);
	set_sensitivity(&hi2c1, &Mpu6050, 0, 0);
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
	while (1) {
		if (key == basicMode) {
			basicScreen();
		} else if (key == bookMode) {
			bookScreen();
		} else if (key == readMode) {
			readScreen();
		} else if (key == settingMode) {
			settingScreen();
		}
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
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
  RCC_PeriphCLKInitTypeDef PeriphClkInit = {0};

  /** Configure the main internal regulator output voltage
  */
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);
  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
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
  /** Initializes the CPU, AHB and APB buses clocks
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
  PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_I2C1;
  PeriphClkInit.I2c1ClockSelection = RCC_I2C1CLKSOURCE_PCLK1;
  if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInit) != HAL_OK)
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
  hadc.Init.ExternalTrigConvEdge = ADC_EXTERNALTRIGCONVEDGE_RISING;
  hadc.Init.ExternalTrigConv = ADC_EXTERNALTRIGCONV_T2_TRGO;
  hadc.Init.DMAContinuousRequests = ENABLE;
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
  /* USER CODE BEGIN ADC_Init 2 */

  /* USER CODE END ADC_Init 2 */

}

/**
  * @brief I2C1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_I2C1_Init(void)
{

  /* USER CODE BEGIN I2C1_Init 0 */

  /* USER CODE END I2C1_Init 0 */

  /* USER CODE BEGIN I2C1_Init 1 */

  /* USER CODE END I2C1_Init 1 */
  hi2c1.Instance = I2C1;
  hi2c1.Init.Timing = 0x00100413;
  hi2c1.Init.OwnAddress1 = 0;
  hi2c1.Init.AddressingMode = I2C_ADDRESSINGMODE_7BIT;
  hi2c1.Init.DualAddressMode = I2C_DUALADDRESS_DISABLE;
  hi2c1.Init.OwnAddress2 = 0;
  hi2c1.Init.OwnAddress2Masks = I2C_OA2_NOMASK;
  hi2c1.Init.GeneralCallMode = I2C_GENERALCALL_DISABLE;
  hi2c1.Init.NoStretchMode = I2C_NOSTRETCH_DISABLE;
  if (HAL_I2C_Init(&hi2c1) != HAL_OK)
  {
    Error_Handler();
  }
  /** Configure Analogue filter
  */
  if (HAL_I2CEx_ConfigAnalogFilter(&hi2c1, I2C_ANALOGFILTER_ENABLE) != HAL_OK)
  {
    Error_Handler();
  }
  /** Configure Digital filter
  */
  if (HAL_I2CEx_ConfigDigitalFilter(&hi2c1, 0) != HAL_OK)
  {
    Error_Handler();
  }
  /** I2C Fast mode Plus enable
  */
  HAL_I2CEx_EnableFastModePlus(I2C_FASTMODEPLUS_I2C1);
  /* USER CODE BEGIN I2C1_Init 2 */

  /* USER CODE END I2C1_Init 2 */

}

/**
  * @brief SPI1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_SPI1_Init(void)
{

  /* USER CODE BEGIN SPI1_Init 0 */

  /* USER CODE END SPI1_Init 0 */

  /* USER CODE BEGIN SPI1_Init 1 */

  /* USER CODE END SPI1_Init 1 */
  /* SPI1 parameter configuration*/
  hspi1.Instance = SPI1;
  hspi1.Init.Mode = SPI_MODE_MASTER;
  hspi1.Init.Direction = SPI_DIRECTION_2LINES;
  hspi1.Init.DataSize = SPI_DATASIZE_8BIT;
  hspi1.Init.CLKPolarity = SPI_POLARITY_LOW;
  hspi1.Init.CLKPhase = SPI_PHASE_1EDGE;
  hspi1.Init.NSS = SPI_NSS_SOFT;
  hspi1.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_4;
  hspi1.Init.FirstBit = SPI_FIRSTBIT_MSB;
  hspi1.Init.TIMode = SPI_TIMODE_DISABLE;
  hspi1.Init.CRCCalculation = SPI_CRCCALCULATION_DISABLE;
  hspi1.Init.CRCPolynomial = 7;
  if (HAL_SPI_Init(&hspi1) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN SPI1_Init 2 */

  /* USER CODE END SPI1_Init 2 */

}

/**
  * @brief TIM2 Initialization Function
  * @param None
  * @retval None
  */
static void MX_TIM2_Init(void)
{

  /* USER CODE BEGIN TIM2_Init 0 */

  /* USER CODE END TIM2_Init 0 */

  TIM_ClockConfigTypeDef sClockSourceConfig = {0};
  TIM_MasterConfigTypeDef sMasterConfig = {0};

  /* USER CODE BEGIN TIM2_Init 1 */

  /* USER CODE END TIM2_Init 1 */
  htim2.Instance = TIM2;
  htim2.Init.Prescaler = 32-1;
  htim2.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim2.Init.Period = 1000-1;
  htim2.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim2.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
  if (HAL_TIM_Base_Init(&htim2) != HAL_OK)
  {
    Error_Handler();
  }
  sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
  if (HAL_TIM_ConfigClockSource(&htim2, &sClockSourceConfig) != HAL_OK)
  {
    Error_Handler();
  }
  sMasterConfig.MasterOutputTrigger = TIM_TRGO_UPDATE;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim2, &sMasterConfig) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN TIM2_Init 2 */

  /* USER CODE END TIM2_Init 2 */

}

/**
  * Enable DMA controller clock
  */
static void MX_DMA_Init(void)
{

  /* DMA controller clock enable */
  __HAL_RCC_DMA1_CLK_ENABLE();

  /* DMA interrupt init */
  /* DMA1_Channel1_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(DMA1_Channel1_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(DMA1_Channel1_IRQn);

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
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(SD_CS_GPIO_Port, SD_CS_Pin, GPIO_PIN_SET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(BUZZER_GPIO_Port, BUZZER_Pin, GPIO_PIN_SET);

  /*Configure GPIO pin : SD_CS_Pin */
  GPIO_InitStruct.Pin = SD_CS_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
  HAL_GPIO_Init(SD_CS_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pin : EN_SW_Pin */
  GPIO_InitStruct.Pin = EN_SW_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(EN_SW_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pins : EN_B_Pin EN_A_Pin */
  GPIO_InitStruct.Pin = EN_B_Pin|EN_A_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_FALLING;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /*Configure GPIO pin : BUZZER_Pin */
  GPIO_InitStruct.Pin = BUZZER_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
  HAL_GPIO_Init(BUZZER_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pin : SW2_Pin */
  GPIO_InitStruct.Pin = SW2_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  HAL_GPIO_Init(SW2_GPIO_Port, &GPIO_InitStruct);

  /* EXTI interrupt init*/
  HAL_NVIC_SetPriority(EXTI4_15_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(EXTI4_15_IRQn);

}

/* USER CODE BEGIN 4 */
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin) {
	if (GPIO_Pin == GPIO_PIN_10) {
		if (SW1B == 1) {
			rotarySw = 2;
		}
	}

	if (GPIO_Pin == GPIO_PIN_9) {
		if (SW1A == 1) {
			rotarySw = 1;
		}
	}
}
/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
	/* User can add his own implementation to report the HAL error return state */
	__disable_irq();
	while (1) {
	}
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
	 ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */

