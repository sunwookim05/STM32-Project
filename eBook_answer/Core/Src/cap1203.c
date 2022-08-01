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
	// ����
	uint8_t dt[2];
	setSensitivity(sensitivity);
	//���ͷ�Ʈ �ο��̺� �������� ����
	dt[0] = INTERRUPT_ENABLE; // �������� �ּ�
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
	//���� ���� �������ͼ���
	uint8_t dt[2];
	dt[0] = SENSITIVITY_CONTROL; //�������� �ּ�
	dt[1] = (sensitivity << 4) | 0x07; // 0111 ���� 1��, ���� 4��Ʈ�� �� ��ġ ���� �Ǵ� ������ ����x
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

