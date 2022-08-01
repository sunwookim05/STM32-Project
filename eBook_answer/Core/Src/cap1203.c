/*
 * cap1203.c
 *
 *  Created on: 2022. 1. 24.
 *      Author: tls123
 */
#include "cap1203.h"
#include "stm32l0xx_hal.h"

I2C_HandleTypeDef hi2c1;

void cap1203_init(uint8_t sensitivity) {
	// 세팅
	uint8_t dt[2];
	setSensitivity(sensitivity);
	//인터럽트 인에이블 레지스터 설정
	dt[0] = INTERRUPT_ENABLE; // 레지스터 주소
	dt[1] = 0x07; // cs3, cs2, cs1
	HAL_I2C_Master_Transmit(&hi2c1, CAP1203_I2C_ADDR << 1, dt, 2, 10);
	dt[0] = MAIN_CONTROL;
	dt[1] = 0x00;
	HAL_I2C_Master_Transmit(&hi2c1, CAP1203_I2C_ADDR << 1, dt, 2, 10);
	dt[0] = REPEAT_RATE_ENABLE;
	dt[1] = 0x00;
	HAL_I2C_Master_Transmit(&hi2c1, CAP1203_I2C_ADDR << 1, dt, 2, 10);
	dt[0] = MULTIPLE_TOUCH_CONFIG;
	dt[1] = 0x83;
	HAL_I2C_Master_Transmit(&hi2c1, CAP1203_I2C_ADDR << 1, dt, 2, 10);
}

void setSensitivity(uint8_t sensitivity) {
	//감도 제어 레지스터설정
	uint8_t dt[2];
	dt[0] = SENSITIVITY_CONTROL; //레지스터 주소
	dt[1] = (sensitivity << 4) | 0x07; // 0111 감도 1배, 하위 4비트는 ㅌ ㅓ치 감지 또는 감도에 영향x
	HAL_I2C_Master_Transmit(&hi2c1, CAP1203_I2C_ADDR << 1, dt, 2, 10);
}

uint8_t cap1203_getinputvalue(uint8_t *temp) {
	uint8_t dt[2];

	dt[0] = SENSOR_INPUT_STATUS;
	HAL_I2C_Master_Transmit(&hi2c1, CAP1203_I2C_ADDR << 1, dt, 1, 10);
	HAL_I2C_Master_Receive(&hi2c1, CAP1203_I2C_ADDR << 1, temp, 1, 1000);
	dt[0] = MAIN_CONTROL;
	dt[1] = 0x00;
	HAL_I2C_Master_Transmit(&hi2c1, CAP1203_I2C_ADDR << 1, dt, 2, 10);
}

