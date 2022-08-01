/* USER CODE BEGIN Header */
/**
 ******************************************************************************
 * @file           : main.c
 * @brief          : Main program body
 ******************************************************************************
 ** This notice applies to any and all portions of this file
 * that are not between comment pairs USER CODE BEGIN and
 * USER CODE END. Other portions of this file, whether
 * inserted by the user or by software development tools
 * are owned by their respective copyright owners.
 *
 * COPYRIGHT(c) 2019 STMicroelectronics
 *
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 *   1. Redistributions of source code must retain the above copyright notice,
 *      this list of conditions and the following disclaimer.
 *   2. Redistributions in binary form must reproduce the above copyright notice,
 *      this list of conditions and the following disclaimer in the documentation
 *      and/or other materials provided with the distribution.
 *   3. Neither the name of STMicroelectronics nor the names of its contributors
 *      may be used to endorse or promote products derived from this software
 *      without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
 * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 ******************************************************************************
 */
/* USER CODE END Header */

/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "adc.h"
#include "dma.h"
#include "i2c.h"
#include "tim.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "lcd_user.h"
#include <stdio.h>
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
#define REV(x) HAL_GPIO_WritePin(LED1_GPIO_Port,LED1_Pin,(x)?GPIO_PIN_SET:GPIO_PIN_RESET)
#define LOCK(x) HAL_GPIO_WritePin(LED2_GPIO_Port,LED2_Pin,(x)?GPIO_PIN_SET:GPIO_PIN_RESET)
#define FWD(x) HAL_GPIO_WritePin(LED3_GPIO_Port,LED3_Pin,(x)?GPIO_PIN_SET:GPIO_PIN_RESET)
#define DETERGENT(x) HAL_GPIO_WritePin(LED4_GPIO_Port,LED4_Pin,(x)?GPIO_PIN_SET:GPIO_PIN_RESET)
#define SUPPLY(x) HAL_GPIO_WritePin(LED5_GPIO_Port,LED5_Pin,(x)?GPIO_PIN_SET:GPIO_PIN_RESET)
#define HEATING(x) HAL_GPIO_WritePin(LED6_GPIO_Port,LED6_Pin,(x)?GPIO_PIN_SET:GPIO_PIN_RESET)
#define SOFT(x) HAL_GPIO_WritePin(LED7_GPIO_Port,LED7_Pin,(x)?GPIO_PIN_SET:GPIO_PIN_RESET)
#define DRYER(x) HAL_GPIO_WritePin(LED8_GPIO_Port,LED8_Pin,(x)?GPIO_PIN_SET:GPIO_PIN_RESET)
#define DRAIN(x) HAL_GPIO_WritePin(LED10_GPIO_Port,LED10_Pin,(x)?GPIO_PIN_SET:GPIO_PIN_RESET)
#define HI_SPEED(x) HAL_GPIO_WritePin(LED11_GPIO_Port,LED11_Pin,(x)?GPIO_PIN_SET:GPIO_PIN_RESET)

#define BUZZER(x) HAL_GPIO_WritePin(PZ_GPIO_Port,PZ_Pin,(x)?GPIO_PIN_SET:GPIO_PIN_RESET)

#define SW_ENTER HAL_GPIO_ReadPin(S1_GPIO_Port,S1_Pin)
#define SW_DOWN HAL_GPIO_ReadPin(S2_GPIO_Port,S2_Pin)
#define SW_BACK HAL_GPIO_ReadPin(S3_GPIO_Port,S3_Pin)
#define SW_UP HAL_GPIO_ReadPin(S4_GPIO_Port,S4_Pin)
#define SW_DOOR HAL_GPIO_ReadPin(S5_GPIO_Port,S5_Pin)

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */
int i = 0, timeload = 1, c = 0, au = 0, work = 0, check = 0, revt, l = 0;
int le, we, te, ltem;
char revtime[100];
char revtemp[100];
char time[10];
char days[10];
char tida[3][25];
char mods[3][30];
char lwe[3][30];
char ltime[3][30];
char lvel[3][30];
char lpc[3][30];
char level[100];
char weight[100];
char temp[100];
uint32_t adc[3];
uint32_t tc[10] = { 0, };
uint8_t tem = 0;
uint8_t t = 0;
uint8_t ms = 0;
uint8_t s = 0;
uint8_t m = 0;
uint8_t h = 0;
uint32_t yea = 21;
uint8_t mon = 1;
uint8_t day = 1;

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */
void* memset(void *ptr, int value, size_t num);
/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

void ledoff() {

	REV(0);
	LOCK(0);
	FWD(0);
	DETERGENT(0);
	SUPPLY(0);
	HEATING(0);
	SOFT(0);
	DRYER(0);
	DRAIN(0);
	HI_SPEED(0);

}

void st() {
	while (SW_UP == 0) {
		HAL_Delay(75);
	}
	while (SW_DOWN == 0) {
		HAL_Delay(75);
	}
	while (SW_BACK == 0) {
		HAL_Delay(75);
	}
	while (SW_ENTER == 0) {
		HAL_Delay(75);
	}
	BUZZER(1);
	HAL_Delay(100);
	BUZZER(0);
}

void error() {

	while (SW_UP == 0) {
		HAL_Delay(75);
	}
	while (SW_DOWN == 0) {
		HAL_Delay(75);
	}
	while (SW_BACK == 0) {
		HAL_Delay(75);
	}
	while (SW_ENTER == 0) {
		HAL_Delay(75);
	}

	BUZZER(1);
	HAL_Delay(100);
	BUZZER(0);
	HAL_Delay(100);
	BUZZER(1);
	HAL_Delay(100);
	BUZZER(0);

}

int dcount(int i) {

	switch (mon) {
	case 1:
		if (day == 32) {
			day = 1;
			mon++;
		}
		break;
	case 2:
		if (i == 0) {
			if (day == 29) {
				day = 1;
				mon++;
			}
		} else {
			if (day == 30) {
				day = 1;
				mon++;
			}
		}
		break;
	case 3:
		if (day == 32) {
			day = 1;
			mon++;
		}
		break;
	case 4:
		if (day == 31) {
			day = 1;
			mon++;
		}
		break;
	case 5:
		if (day == 32) {
			day = 1;
			mon++;
		}
		break;
	case 6:
		if (day == 31) {
			day = 1;
			mon++;
		}
		break;
	case 7:
		if (day == 32) {
			day = 1;
			mon++;
		}
		break;
	case 8:
		if (day == 32) {
			day = 1;
			mon++;
		}
		break;
	case 9:
		if (day == 31) {
			day = 1;
			mon++;
		}
		break;
	case 10:
		if (day == 32) {
			day = 1;
			mon++;
		}
		break;
	case 11:
		if (day == 31) {
			day = 1;
			mon++;
		}
		break;
	case 12:
		if (day == 32) {

			day = 1;
			mon = 1;
			yea++;
		}
		break;
	}
}

void daygo() {

	int i;
	if (h == 24) {
		h = 0;
		day++;
	}
	if (day >= 28) {
		if (yea % 4 == 0 && yea % 100 == 0 && yea % 400 == 0) {
			//윤년
			i = 1;
		} else if (yea % 4 == 0 && yea % 100 == 0) {
			//평년
			i = 0;
		} else if (yea % 4 == 0) {
			//윤년
			i = 1;
		} else {
			//평년
			i = 0;
		}
	}

	dcount(i);

}

void timego() {
	uint8_t n;

	if (t == 100) {
		t = 0;
		ms++;

		if (ms == 10) {
			ms = 0;
			s++;
		}
		if (s == 60) {
			s = 0;
			m++;
		}
		if (m == 60) {
			m = 0;
			h++;
			daygo();
		}
		sprintf(time, "%02d:%02d:%02d", h, m, s);
		sprintf(days, "%02ld-%02d-%02d", yea, mon, day);

		if (timeload == 1) {

			OLED_ShowStringXY(13, 0, time, 1);
			OLED_ShowStringXY(0, 0, days, 1);

		}
	}

}

void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim) {

	int a;

	if (htim->Instance == htim6.Instance) {
		t++;
		for (a = 0; a < 8; a++)
			tc[i]++;
		timego();
	}
}

void oc() {
	int r, n;

	for (r = 1; r < 8; r++) {
		IIC_Writecmd(0xb0 + r);
		IIC_Writecmd(0x00);
		IIC_Writecmd(0x10);
		for (n = 0; n < 128; n++)
			IIC_Writedata(0);
	}
	OLED_Set_Pos(0, 0);

	if (timeload == 1) {
		OLED_Set_Pos(0, 1);
		for (n = 0; n < 128; n++)
			IIC_Writedata(1);
		OLED_Set_Pos(0, 0);
	}
}

void door() {

	if (SW_DOOR == 0) {

		LOCK(1);
		OLED_ShowStringXY(0, 4, "D:CLOSE", 1);

	} else if (SW_DOOR == 1) {
		if (work == 1) {
			BUZZER(1);
			HAL_Delay(100);
			BUZZER(0);
			HAL_Delay(100);
		}

		LOCK(0);
		OLED_ShowStringXY(0, 4, "D:OPEN   ", 1);

	}

}

void vr() {

	HAL_ADC_Start_DMA(&hadc, (uint32_t*) adc, 3);

	le = adc[0] / 40 - 3;
	we = adc[1] / 227;
	te = adc[2] / 56 - 3;

	if (le <= -1) {
		le = 0;
	}
	if (te <= -1) {
		te = 0;
	}
	if (te >= 66 || we >= 16) {
		BUZZER(1);
		HAL_Delay(100);
		BUZZER(0);
		HAL_Delay(100);

	}

	sprintf(level, "L:%02d", le);
	sprintf(weight, "W:%02d", we);
	sprintf(temp, "T:%02d", te);

	OLED_ShowStringXY(0, 6, level, 1);
	OLED_ShowStringXY(8, 6, weight, 1);
	OLED_ShowStringXY(16, 6, temp, 1);

	if (le >= we * 6 && le != 0) {

		SUPPLY(0);

		if (c == 0) {
			c = 1;
			sprintf(lvel[2], "%s", lvel[1]);
			sprintf(lvel[1], "%s", lvel[0]);
			sprintf(lvel[0], "Water cost:%.2lf m|", tc[5] / 1000 * 0.08);

		}

	}

	door();

}

void vrr() {

	HAL_ADC_Start_DMA(&hadc, (uint32_t*) adc, 3);

	le = adc[0] / 40 - 3;
	we = adc[1] / 227;
	te = adc[2] / 56 - 3;

	if (le <= -1) {
		le = 0;
	}
	if (te <= -1) {
		te = 0;
	}
	if (te >= 66 || we >= 16) {
		BUZZER(1);
		HAL_Delay(100);
		BUZZER(0);
		HAL_Delay(100);

	}

	sprintf(level, "L:%02d", le);
	sprintf(weight, "W:%02d", we);
	sprintf(temp, "T:%02d", te);

	OLED_ShowStringXY(0, 6, level, 1);
	OLED_ShowStringXY(8, 6, weight, 1);
	OLED_ShowStringXY(16, 6, temp, 1);

	if (le >= we * 6 && le != 0) {

		SUPPLY(0);

		if (au == 0) {
			if (c == 0) {
				c = 1;
				sprintf(lvel[2], "%s", lvel[1]);
				sprintf(lvel[1], "%s", lvel[0]);
				sprintf(lvel[0], "Water cost:%.2lf m|", tc[5] / 1000 * 0.08);

			}
		}

	}

	door();

}

void vd() {

	HAL_ADC_Start_DMA(&hadc, (uint32_t*) adc, 3);

	le = adc[0] / 40 - 3;
	we = adc[1] / 227;
	te = adc[2] / 56 - 3;

	if (le <= -1) {
		le = 0;
	}
	if (te <= -1) {
		te = 0;
	}

	if (te >= 66 || we >= 16) {
		BUZZER(1);
		HAL_Delay(100);
		BUZZER(0);
		HAL_Delay(100);

	}

	sprintf(level, "L:%02d", le);
	sprintf(weight, "W:%02d", we);
	sprintf(temp, "T:%02d", te);

	OLED_ShowStringXY(0, 6, level, 1);
	OLED_ShowStringXY(8, 6, weight, 1);
	OLED_ShowStringXY(16, 6, temp, 1);

	door();

}

void swui() {

	switch (i) {

	case 0:
		OLED_ShowStringXY(13, 4, "SET:COLD", 1);
		tem = 20;
		break;

	case 1:
		OLED_ShowStringXY(13, 4, "SET:30}C", 1);
		tem = 30;
		break;

	case 2:
		OLED_ShowStringXY(13, 4, "SET:40}C", 1);
		tem = 40;
		break;

	case 3:
		OLED_ShowStringXY(13, 4, "SET:50}C", 1);
		tem = 50;
		break;

	case 4:
		OLED_ShowStringXY(13, 4, "SET:60}C", 1);
		tem = 60;
		break;

	}

}

int sww(int x) {

	static int j = 1;

	if (x == 1) {
		tc[5] = 0;
		tc[7] = 0;
		check = 1;
		SUPPLY(1);
		DETERGENT(1);
		tc[1] = 0;
		j = 2;
	}

	if (j == 2) {
		if (tc[1] >= 2000) {
			DETERGENT(0);
			return 1;
		}
	}

}

int srw(int x) {

	static int j = 1;

	if (x == 1) {
		tc[5] = 0;
		tc[7] = 0;
		check = 1;
		SUPPLY(1);
		DETERGENT(1);
		tc[1] = 0;
		j = 2;
	}

	if (j == 2) {
		if (tc[1] >= 1000) {
			DETERGENT(0);
			return 1;
		}
	}

	if (tc[7] >= 5000 && le < we * 6) {
		BUZZER(1);
		HAL_Delay(100);
		BUZZER(0);
		HAL_Delay(100);

	}
}

int semid() {

	int x = 1, y = 0, j = 0, z;
	i = 0;
	ledoff();
	door();
	vd();
	tc[7] = 0;

	while (1) {

		if (tc[0] >= 75) {
			tc[0] = 0;
			vd();
			if (tc[2] >= 500) {
				tc[2] = 0;

				if (le < 2 && SW_DOOR == 0) {

					work = 1;
					DRAIN(0);

					if (y == 0) {
						y = 1;
						tc[3] = 0;
					}

					if (tc[3] >= 0 && tc[3] < 3000) {
						FWD(1);
					} else if (tc[3] >= 3000 && tc[3] < 4000) {
						FWD(0);
					} else if (tc[3] >= 4000 && tc[3] < 7000) {
						REV(1);
					} else if (tc[3] >= 7000 && tc[3] < 8000) {
						REV(0);
					} else if (tc[3] >= 8000 && tc[3] < 11000) {
						FWD(1);
					} else if (tc[3] >= 11000 && tc[3] < 20000) {
						HI_SPEED(1);
					} else if (tc[3] >= 20000 && tc[3] < 21000) {
						HI_SPEED(0);
					} else if (tc[3] >= 22000 && tc[3] < 23000) {
						ledoff();
						i = 2;
						work = 0;
						check = 0;
						return 5;
					}
				}
			}
		}

		if (le >= 2) {
			if (check == 0) {
				tc[7] = 0;
				check = 1;
			} else if (check == 1 && tc[7] >= 5000) {
				BUZZER(1);
				HAL_Delay(100);
				BUZZER(0);
				HAL_Delay(100);

			}
			DRAIN(1);

		} else {
			DRAIN(0);

		}

		if (SW_UP == 0) {
			error();

		} else if (SW_DOWN == 0) {
			error();

		}

		if (SW_ENTER == 0) {
			error();

		}

		if (SW_BACK == 0) {

			error();

		}
	}

}

int semir() {

	int x = 1, y = 0, j = 0, z;

	i = 0;
	door();

	while (1) {

		if (tc[0] >= 75) {
			tc[0] = 0;
			vrr();

			if (tc[2] >= 500) {
				tc[2] = 0;

				if (SW_DOOR == 0) {
					j = srw(x);
					if (x == 1) {
						x = 2;

					}
				}

				if (j == 1 && x == 2 && le >= we * 6 && SW_DOOR == 0
						&& le != 0) {

					work = 1;

					if (y == 0) {
						y = 1;
						tc[3] = 0;
					}

					if (tc[3] >= 0 && tc[3] < 2000) {
						FWD(1);
					} else if (tc[3] >= 2000 && tc[3] < 3000) {
						FWD(0);
					} else if (tc[3] >= 3000 && tc[3] < 5000) {
						REV(1);
					} else if (tc[3] >= 5000 && tc[3] < 6000) {
						REV(0);
					} else if (tc[3] >= 6000 && tc[3] < 8000) {
						FWD(1);
					} else if (tc[3] >= 8000 && tc[3] < 9000) {
						FWD(0);
					} else if (tc[3] >= 9000 && tc[3] < 11000) {
						REV(1);
					} else if (tc[3] >= 11000 && tc[3] < 12000) {
						REV(0);
						ledoff();
						i = 1;
						c = 0;
						work = 0;
						check = 0;
						return 5;
					}
				}
			}
		}

		if (check == 1) {
			if (tc[7] >= 5000 && le < we * 6) {
				BUZZER(1);
				HAL_Delay(100);
				BUZZER(0);
				HAL_Delay(100);

			}
		}

		if (SW_UP == 0) {
			error();

		} else if (SW_DOWN == 0) {
			error();
		}

		if (SW_ENTER == 0) {
			error();

		}

		if (SW_BACK == 0) {

			error();

		}
	}

}

int semiw() {

	int x = 0, y = 0, j = 0, ch = 0;

	i = 0;
	OLED_ShowStringXY(15, 2, "SETT", 1);
	door();
	swui();

	while (1) {

		if (tc[0] >= 75) {
			tc[0] = 0;
			vr();
			if (tc[2] >= 500) {
				tc[2] = 0;
				j = sww(x);
				if (x == 1) {
					x = 2;

				}

				if (j == 1 && x == 2 && le >= we * 6 && te >= tem
						&& SW_DOOR == 0 && le != 0) {

					work = 1;

					if (y == 0) {
						y = 1;
						tc[3] = 0;
					}

					if (tc[3] >= 0 && tc[3] < 2000) {
						FWD(1);
					} else if (tc[3] >= 2000 && tc[3] < 3000) {
						FWD(0);
					} else if (tc[3] >= 3000 && tc[3] < 5000) {
						REV(1);
					} else if (tc[3] >= 5000 && tc[3] < 6000) {
						REV(0);
					} else if (tc[3] >= 6000 && tc[3] < 8000) {
						FWD(1);
					} else if (tc[3] >= 8000 && tc[3] < 9000) {
						FWD(0);
					} else if (tc[3] >= 9000 && tc[3] < 11000) {
						REV(1);
					} else if (tc[3] >= 11000 && tc[3] < 12000) {
						REV(0);
					} else if (tc[3] >= 12000 && tc[3] < 14000) {
						FWD(1);
					} else if (tc[3] >= 14000 && tc[3] < 15000) {
						FWD(0);
					} else if (tc[3] >= 15000 && tc[3] < 17000) {
						REV(1);
					} else if (tc[3] >= 17000 && tc[3] < 18000) {
						ledoff();
						i = 0;
						c = 0;
						work = 0;
						check = 0;
						return 5;
					}
				}
			}

		}

		if (te < tem && le >= we * 6 && le != 0 && we != 0) {
			if (ch == 0) {
				tc[6] = 0;
				ch = 1;
			}
			HEATING(1);

		}

		if (tc[6] < 5000 && te < tem) {
			BUZZER(1);
			HAL_Delay(100);
			BUZZER(0);
			HAL_Delay(100);

		}

		if (te >= tem) {
			if (au == 0) {
				sprintf(lpc[2], "%s", lpc[1]);
				sprintf(lpc[1], "%s", lpc[0]);
				sprintf(lpc[0], "Power cost:%.2lf kw.h",
						12 * 0.05 + tc[6] / 1000 * 0.2);

			}
			ltem = tc[6];
			HEATING(0);

		}

		if (check == 1) {
			if (tc[7] >= 5000 && le < we * 6) {
				BUZZER(1);
				HAL_Delay(100);
				BUZZER(0);
				HAL_Delay(100);

			}
		}

		if (SW_UP == 0) {
			st();
			i++;
			if (i >= 5) {
				i = 4;
			}
			swui();

		} else if (SW_DOWN == 0) {
			st();
			i--;
			if (i <= -1) {
				i = 0;
			}
			swui();

		}

		if (SW_ENTER == 0) {
			st();
			if (SW_DOOR == 0) {
				OLED_ShowStringXY(15, 2, "     ", 1);
				x++;

			}

		}

		if (SW_BACK == 0) {

			error();

		}

	}

}

int sautodry() {

	int x = 1, y = 0, j = 0, z;

	i = 0;
	OLED_ShowStringXY(0, 2, "Auto-Dry ", 1);
	door();

	while (1) {

		if (tc[0] >= 75) {
			tc[0] = 0;
			vd();
			if (tc[2] >= 500) {
				tc[2] = 0;

				if (le < 2 && SW_DOOR == 0) {
					work = 1;
					DRAIN(0);

					if (y == 0) {
						y = 1;
						tc[3] = 0;
					}

					if (tc[3] >= 0 && tc[3] < 2000) {
						FWD(1);
						DRYER(1);
					} else if (tc[3] >= 2000 && tc[3] < 3000) {
						FWD(0);
					} else if (tc[3] >= 3000 && tc[3] < 5000) {
						REV(1);
					} else if (tc[3] >= 5000 && tc[3] < 6000) {
						REV(0);
					} else if (tc[3] >= 6000 && tc[3] < 8000) {
						FWD(1);
					} else if (tc[3] >= 8000 && tc[3] < 9000) {
						FWD(0);
					} else if (tc[3] >= 9000 && tc[3] < 11000) {
						REV(1);
						DRYER(1);
					} else if (tc[3] >= 11000 && tc[3] < 12000) {
						REV(0);
						ledoff();
						i = 1;
						work = 0;
						return 5;
					}
				}
			}
		}

		if (le >= 2) {
			DRAIN(1);

		} else {
			DRAIN(0);

		}

		if (SW_UP == 0) {
			error();

		} else if (SW_DOWN == 0) {
			error();
		}

		if (SW_ENTER == 0) {
			error();

		}

		if (SW_BACK == 0) {

			error();

		}
	}
}

void semirepui(int j) {

	OLED_ShowStringXY(0, 3, "-OK       ", 0 != j);
	OLED_ShowStringXY(0, 5, "-BACK     ", 1 != j);

}

int semirep() {

	int j = 0;
	semirepui(j);

	while (1) {

		if (SW_UP == 0) {
			st();
			j--;
			if (j <= -1) {
				j = 0;
			}
			semirepui(j);

		} else if (SW_DOWN == 0) {
			st();
			j++;
			if (j >= 2) {
				j = 1;
			}
			semirepui(j);

		}

		if (SW_ENTER == 0) {
			st();
			oc();
			if (j == 1) {
				i = 1;
				return 1;

			} else {
				return 6;

			}

		}

		if (SW_BACK == 0) {
			st();
			i = 1;
			return 1;

		}

	}

}

int sauto() {

	i = 0;
	au = 1;
	OLED_ShowStringXY(0, 2, "Auto-Wash", 1);
	semiw();
	semid();
	semir();
	semid();
	semir();
	semid();
	sautodry();

	au = 0;
	i = 0;
	return 1;

}

void sautoui(int j) {

	OLED_ShowStringXY(0, 1, "Auto", 1);
	OLED_ShowStringXY(0, 3, "-START    ", 0 != j);
	OLED_ShowStringXY(0, 5, "-BACK     ", 1 != j);

}

int sautorep() {

	int j = 0;
	sautoui(j);

	while (1) {

		if (SW_UP == 0) {
			st();
			j--;
			if (j <= -1) {
				j = 0;
			}
			sautoui(j);

		} else if (SW_DOWN == 0) {
			st();
			j++;
			if (j >= 2) {
				j = 1;
			}
			sautoui(j);

		}

		if (SW_ENTER == 0) {
			st();
			oc();
			if (j == 1) {
				i = 0;
				return 0;

			} else {
				return 7;

			}

		}

		if (SW_BACK == 0) {

			st();
			i = 0;
			return 0;

		}

	}

}

void semiui() {
	OLED_ShowStringXY(0, 3, "-Wash     ", 0 != i);
	OLED_ShowStringXY(0, 5, "-RINSE    ", 1 != i);
	OLED_ShowStringXY(0, 7, "-DRY      ", 2 != i);

}

int semi() {

	i = 0;
	OLED_ShowStringXY(0, 1, "Semi", 1);
	semiui();

	while (1) {

		if (SW_UP == 0) {
			st();
			i--;
			if (i <= -1) {
				i = 0;
			}
			semiui();

		} else if (SW_DOWN == 0) {
			st();
			i++;
			if (i >= 3) {
				i = 2;
			}
			semiui();

		}

		if (SW_ENTER == 0) {
			st();
			return 5;

		}

		if (SW_BACK == 0) {
			st();
			oc();
			i = 0;
			return 3;
		}

	}

}

void leds(int j) {

	switch (i) {

	case 0:
		FWD(j != 1);
		break;

	case 1:
		REV(j != 1);
		break;

	case 2:
		DRYER(j != 1);
		break;

	case 3:
		HEATING(j != 1);
		break;

	case 4:
		HI_SPEED(j != 1);
		break;

	case 5:
		SUPPLY(j != 1);
		break;

	case 6:
		DRAIN(j != 1);
		break;

	case 7:
		LOCK(j != 1);
		break;

	case 8:
		DETERGENT(j != 1);
		break;

	case 9:
		SOFT(j != 1);
		break;

	}

}

void led(int j) {

	OLED_ShowStringXY(0, 5, "-ON       ", 0 != j);
	OLED_ShowStringXY(0, 7, "-OFF      ", 1 != j);

}

int manualleds() {
	int j = 0;

	OLED_ShowStringXY(9, 3, "Run", 1);
	led(j);

	while (1) {

		if (SW_UP == 0) {
			st();
			j--;
			if (j <= -1) {
				j = 0;
			}
			led(j);

		} else if (SW_DOWN == 0) {
			st();
			j++;
			if (j >= 2) {
				j = 1;
			}
			led(j);

		} else if (SW_BACK == 0) {
			st();
			oc();
			return 4;
		}

		if (SW_ENTER == 0) {
			st();
			leds(j);
			BUZZER(1);
			HAL_Delay(300);
			BUZZER(0);

		}

	}

}

void manuals() {

	if (i == 3 || i == 4 || i == 8 || i == 9) {
		OLED_ShowStringXY(0, 3, "-HEAT     ", 3 != i);
		OLED_ShowStringXY(0, 5, "-HI-SPEED ", 4 != i);
		OLED_ShowStringXY(11, 3, "-DETE    ", 8 != i);
		OLED_ShowStringXY(11, 5, "-SOFT    ", 9 != i);

	} else {

		OLED_ShowStringXY(0, 3, "-FWD      ", 0 != i);
		OLED_ShowStringXY(0, 5, "-REV      ", 1 != i);
		OLED_ShowStringXY(0, 7, "-DRYER    ", 2 != i);
		OLED_ShowStringXY(11, 3, "-Win     ", 5 != i);
		OLED_ShowStringXY(11, 5, "-Wout    ", 6 != i);
		OLED_ShowStringXY(11, 7, "-LOCK    ", 7 != i);

	}

}

int manual() {

	i = 0;
	OLED_ShowStringXY(0, 1, "Manual", 1);
	manuals();

	while (1) {

		if (SW_UP == 0) {
			st();
			i--;
			if (i <= -1) {
				i = 0;
			}
			if (i == 2 || i == 4 || i == 7) {
				oc();
				OLED_ShowStringXY(0, 1, "Manual", 1);

			}
			manuals();

		} else if (SW_DOWN == 0) {
			st();
			i++;
			if (i >= 10) {
				i = 9;
			}
			if (i == 3 || i == 5 || i == 8) {
				oc();
				OLED_ShowStringXY(0, 1, "Manual", 1);

			}
			manuals();

		}

		if (SW_ENTER == 0) {
			st();
			return 2;

		}

		if (SW_BACK == 0) {
			st();
			oc();
			return 3;
		}

	}

}

int modes() {

	OLED_ShowStringXY(0, 3, "-AUTO     ", 0 != i);
	OLED_ShowStringXY(0, 5, "-SEMI-AUTO", 1 != i);
	OLED_ShowStringXY(0, 7, "-MANUAL   ", 2 != i);

}

int mode() {

	i = 0;
	OLED_ShowStringXY(0, 1, "Mode", 1);
	modes();

	while (1) {

		if (SW_UP == 0) {
			st();
			i--;
			if (i <= -1) {
				i = 0;
			}
			modes();

		} else if (SW_DOWN == 0) {
			st();
			i++;
			if (i >= 3) {
				i = 2;
			}
			modes();

		}

		if (SW_ENTER == 0) {
			st();
			return 1;

		}

		if (SW_BACK == 0) {
			st();
			oc();
			i = 0;
			return -1;
		}

	}

}

int alog() {

	timeload = 0;

	OLED_ShowStringXY(0, 0, "LOG:", 1);
	OLED_ShowStringXY(4, 0, tida[i], 1);
	OLED_ShowStringXY(0, 1, mods[i], 1);
	OLED_ShowStringXY(0, 2, ltime[i], 1);
	OLED_ShowStringXY(12, 2, lwe[i], 1);
	OLED_ShowStringXY(0, 4, lvel[i], 1);
	OLED_ShowStringXY(0, 6, lpc[i], 1);

	while (1) {

		if (SW_UP == 0) {
			error();

		} else if (SW_DOWN == 0) {
			error();

		}

		if (SW_ENTER == 0) {
			error();

		}

		if (SW_BACK == 0) {
			st();
			OLED_Clear();
			i = 2;
			timeload = 1;
			return 0;
		}

	}

}

void logsui() {

	OLED_ShowStringXY(0, 3, "1.", 0 != i);
	OLED_ShowStringXY(0, 5, "2.", 1 != i);
	OLED_ShowStringXY(0, 7, "3.", 2 != i);
	OLED_ShowStringXY(2, 3, tida[0], 0 != i);
	OLED_ShowStringXY(2, 5, tida[1], 1 != i);
	OLED_ShowStringXY(2, 7, tida[2], 2 != i);

}

//sprintf(tida[0], "%02ld-%02d-%02d %02d:%02d:%02d", yea, mon, day, h, m, s);
//sprintf(tida[1], "%02ld-%02d-%02d %02d:%02d:%02d", yea, mon, day, h, m, s);
//sprintf(tida[2], "%02ld-%02d-%02d %02d:%02d:%02d", yea, mon, day, h, m, s);

int logs() {

	i = 0;
	timeload = 0;
	OLED_ShowStringXY(7, 0, "LOG MENU", 1);
	logsui();

	while (1) {

		if (SW_UP == 0) {
			st();
			i--;
			if (i <= -1) {
				i = 0;
			}
			logsui();

		} else if (SW_DOWN == 0) {
			st();
			i++;
			if (i >= 3) {
				i = 2;
			}
			logsui();

		}

		if (SW_ENTER == 0) {
			st();
			OLED_Clear();

			return 8;

		}

		if (SW_BACK == 0) {
			st();
			OLED_Clear();
			i = 0;
			return -1;
		}

	}

}

void wait() {
	int a;
	a = 60000 * l;
	while (1) {

	}

}

void revtiui() {

	if (l == 0) {
		sprintf(revtime, "TIME:NOW       ");
	} else {
		sprintf(revtime, "TIME:%02dminute", l);
	}

	OLED_ShowBigStringXY(0, 4, revtime, 1);

}

void rev() {
	timeload = 0;
	OLED_Clear();
	OLED_ShowBigStringXY(2, 0, "RESERVATION", 1);
	revtiui();

	while (1) {

		if (SW_UP == 0) {
			st();
			l++;
			if (l >= 11) {
				l = 10;
			}
			revtiui();

		} else if (SW_DOWN == 0) {
			st();
			l--;
			if (l <= -1) {
				l = 0;
			}
			revtiui();
		}

		if (SW_ENTER == 0) {
			st();
			if (l >= 1) {
				wait();
			}
			timeload = 1;
			OLED_Clear();
			return;
		}

		if (SW_BACK == 0) {
			error();
		}

	}

}

void wg(int j) {

	int n;

	while (1) {

		if (j == -1) {

			if (timeload == 1) {
				OLED_Set_Pos(0, 1);
				for (n = 0; n < 128; n++)
					IIC_Writedata(1);
				OLED_Set_Pos(0, 0);
			}

			OLED_ShowStringXY(0, 2, "Menu", 1);
			OLED_ShowStringXY(0, 4, "-ENTER    ", 0);
			OLED_ShowStringXY(0, 6, "-LOG      ", 1);

			return;
		}

		if (j == 0) {

			if (i == 0) {
				oc();
				j = mode();

			} else {
				OLED_Clear();
				j = logs();
				timeload = 1;

			}

		}

		if (j == 1) {

			if (i == 0) {
				oc();
				j = sautorep();

			} else if (i == 1) {
				oc();
				j = semi();

			} else {
				oc();
				j = manual();

			}

		}

		if (j == 2) {
			oc();

			switch (i) {

			case 0:
				OLED_ShowStringXY(0, 1, "Manual-FWD", 1);
				break;

			case 1:
				OLED_ShowStringXY(0, 1, "Manual-REV", 1);
				break;

			case 2:
				OLED_ShowStringXY(0, 1, "Manual-DRYER", 1);
				break;

			case 3:
				OLED_ShowStringXY(0, 1, "Manual-HEAT", 1);
				break;

			case 4:
				OLED_ShowStringXY(0, 1, "Manual-HI-SPEED", 1);
				break;

			case 5:
				OLED_ShowStringXY(0, 1, "Manual-Win", 1);
				break;

			case 6:
				OLED_ShowStringXY(0, 1, "Manual-Wout", 1);
				break;

			case 7:
				OLED_ShowStringXY(0, 1, "Manual-LOCK", 1);
				break;

			case 8:
				OLED_ShowStringXY(0, 1, "Manual-DETE", 1);
				break;

			case 9:
				OLED_ShowStringXY(0, 1, "Manual-SOFT", 1);
				break;

			}
			j = manualleds();

		}

		if (j == 3) {

			j = mode();

		}

		if (j == 4) {

			j = manual();

		}

		if (j == 5) {
			oc();

			switch (i) {

			case 0:
				OLED_ShowStringXY(0, 1, "Semi-Wash", 1);

				break;

			case 1:
				OLED_ShowStringXY(0, 1, "Semi-Rinse", 1);

				break;

			case 2:
				OLED_ShowStringXY(0, 1, "Semi-Dry", 1);

				break;

			}

			j = semirep();

		}

		if (j == 6) {

			oc();
			rev();
			oc();
			sprintf(tida[2], "%s", tida[1]);
			sprintf(tida[1], "%s", tida[0]);
			sprintf(tida[0], "%02ld-%02d-%02d %02d:%02d:%02d", yea, mon, day, h,
					m, s);

			tc[4] = 0;

			switch (i) {

			case 0:
				OLED_ShowStringXY(0, 2, "Semi-Wash", 1);
				sprintf(mods[2], "%s", mods[1]);
				sprintf(mods[1], "%s", mods[0]);
				sprintf(mods[0], "Semi-Wash");
				j = semiw();

				break;

			case 1:
				OLED_ShowStringXY(0, 2, "Semi-Rinse", 1);
				sprintf(mods[2], "%s", mods[1]);
				sprintf(mods[1], "%s", mods[0]);
				sprintf(mods[0], "Semi-Rinse");
				j = semir();
				sprintf(lpc[2], "%s", lpc[1]);
				sprintf(lpc[1], "%s", lpc[0]);
				sprintf(lpc[0], "Power cost:%.2lf kw.h", 8 * 0.05);

				break;

			case 2:
				OLED_ShowStringXY(0, 2, "Semi-Dry", 1);
				sprintf(mods[2], "%s", mods[1]);
				sprintf(mods[1], "%s", mods[0]);
				sprintf(mods[0], "Semi-Dry");
				j = semid();
				sprintf(lpc[2], "%s", lpc[1]);
				sprintf(lpc[1], "%s", lpc[0]);
				sprintf(lpc[0], "Power cost:%.2lf kw.h", 15 * 0.05 + 5 * 0.08);

				break;

			}

			sprintf(ltime[2], "%s", ltime[1]);
			sprintf(ltime[1], "%s", ltime[0]);
			sprintf(ltime[0], "DUR:%d", tc[4] / 1000);

			sprintf(lwe[2], "%s", lwe[1]);
			sprintf(lwe[1], "%s", lwe[0]);
			sprintf(lwe[0], "W:%d kg", we);

			BUZZER(1);
			HAL_Delay(100);
			BUZZER(0);
			HAL_Delay(100);
			BUZZER(1);
			HAL_Delay(100);
			BUZZER(0);
			HAL_Delay(100);
			BUZZER(1);
			HAL_Delay(100);
			BUZZER(0);

		}

		if (j == 7) {
			oc();
			rev();
			oc();
			sprintf(tida[2], "%s", tida[1]);
			sprintf(tida[1], "%s", tida[0]);
			sprintf(tida[0], "%02ld-%02d-%02d %02d:%02d:%02d", yea, mon, day, h,
					m, s);
			sprintf(mods[2], "%s", mods[1]);
			sprintf(mods[1], "%s", mods[0]);
			sprintf(mods[0], "AUTO");

			tc[4] = 0;
			j = sauto();

			sprintf(ltime[2], "%s", ltime[1]);
			sprintf(ltime[1], "%s", ltime[0]);
			sprintf(ltime[0], "DUR:%d", tc[4] / 1000);

			sprintf(lwe[2], "%s", lwe[1]);
			sprintf(lwe[1], "%s", lwe[0]);
			sprintf(lwe[0], "W:%d kg", we);

			sprintf(lpc[2], "%s", lpc[1]);
			sprintf(lpc[1], "%s", lpc[0]);
			sprintf(lpc[0], "Power cost:%.2lf kw.h",
					114 * 0.05 + ltem / 1000 * 0.2 + 15 * 0.08 + 9 * 0.15);

			BUZZER(1);
			HAL_Delay(100);
			BUZZER(0);
			HAL_Delay(100);
			BUZZER(1);
			HAL_Delay(100);
			BUZZER(0);
			HAL_Delay(100);
			BUZZER(1);
			HAL_Delay(100);
			BUZZER(0);

		}

		if (j == 8) {
			OLED_Clear();
			j = alog();
			timeload = 1;

		}

	}

}

int menu() {

	OLED_ShowStringXY(0, 4, "-ENTER    ", 0 != i);
	OLED_ShowStringXY(0, 6, "-LOG      ", 1 != i);

}

/* USER CODE END 0 */

/**
 * @brief  The application entry point.
 * @retval int
 */
int main(void) {
	/* USER CODE BEGIN 1 */
	int n;
	i = 0;
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
	MX_ADC_Init();
	MX_I2C1_Init();
	MX_TIM6_Init();
	/* USER CODE BEGIN 2 */

	ledoff();
	BUZZER(0);
	lcd_init(&hi2c1);
//	OLED_ShowStringXY(7, 3, "\fWelcome\n\n   Washing Machine", 1);
	OLED_ShowBigStringXY(6, 2, "2021", 1);
	OLED_ShowBigStringXY(2, 4, "World ", 1);
	OLED_ShowBigStringXY(8, 4, "Skill", 0);

	HAL_Delay(2000);
	OLED_Clear();

	HAL_TIM_Base_Start_IT(&htim6);

	if (timeload == 1) {
		OLED_Set_Pos(0, 1);
		for (n = 0; n < 128; n++)
			IIC_Writedata(1);
		OLED_Set_Pos(0, 0);
	}

	/* USER CODE END 2 */

	OLED_ShowStringXY(0, 2, "Menu", 1);
	menu();

	sprintf(tida[0], "YY-MM-DD hh:mm:ss");
	sprintf(tida[1], "YY-MM-DD hh:mm:ss");
	sprintf(tida[2], "YY-MM-DD hh:mm:ss");
	sprintf(revtime, "TIME:NOW     ");

	/* Infinite loop */
	/* USER CODE BEGIN WHILE */
	while (1) {
		/* USER CODE END WHILE */
		if (SW_UP == 0) {
			st();
			i--;
			if (i <= -1) {
				i = 0;
			}
			menu();

		} else if (SW_DOWN == 0) {
			st();
			i++;
			if (i >= 2) {
				i = 1;
			}
			menu();

		}

		if (SW_ENTER == 0) {
			st();
			wg(0);

		}

		if (SW_BACK == 0) {

			error();

		}
		/* USER CODE BEGIN 3 */
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
	RCC_PeriphCLKInitTypeDef PeriphClkInit = { 0 };

	/**Configure the main internal regulator output voltage
	 */
	__HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);
	/**Initializes the CPU, AHB and APB busses clocks
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
	/**Initializes the CPU, AHB and APB busses clocks
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
	PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_I2C1;
	PeriphClkInit.I2c1ClockSelection = RCC_I2C1CLKSOURCE_PCLK1;
	if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInit) != HAL_OK) {
		Error_Handler();
	}
}

/* USER CODE BEGIN 4 */

/* USER CODE END 4 */

/**
 * @brief  This function is executed in case of error occurrence.
 * @retval None9
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
