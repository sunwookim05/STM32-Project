/*
 * MPU6050.c
 *
 *  Created on: 2020. 5. 22.
 *      Author: bbb
 */

#include "MPU6050.h"
#include "math.h"

I2C_HandleTypeDef hi2c1;

void init_MPU6050(void) {
	uint8_t temp = 0;
	uint8_t dt[2];
	dt[0] = MPU6050_RA_WHO_AM_I;
	HAL_I2C_Master_Transmit(&hi2c1, MPU6050_ADDRESS_AD0_LOW << 1, dt, 1, 10);
	HAL_I2C_Master_Receive(&hi2c1, MPU6050_ADDRESS_AD0_LOW << 1, &temp, 1,
			1000);
	if (temp != 0x68)
		return;

	dt[0] = MPU6050_RA_PWR_MGMT_1;
	dt[1] = 0x00;
	HAL_I2C_Master_Transmit(&hi2c1, MPU6050_ADDRESS_AD0_LOW << 1, dt, 2, 1000);

	dt[0] = MPU6050_RA_SMPLRT_DIV;
	dt[1] = 7;
	HAL_I2C_Master_Transmit(&hi2c1, MPU6050_ADDRESS_AD0_LOW << 1, dt, 2, 1000);
}

void set_sensitivity(I2C_HandleTypeDef *hi2c, mpu6050 *__my_mpu6050,
		int gyro_range_you_want, int accel_range_you_want) {

	uint8_t slave_address = 0x69 << 1;
	uint8_t data_to_write[3];

	data_to_write[0] = 0x1B;
	data_to_write[1] = gyro_range_you_want << 3;
	data_to_write[2] = accel_range_you_want << 3;

	HAL_I2C_Master_Transmit(hi2c, slave_address, data_to_write, 3, 1000);

	switch (gyro_range_you_want) {
	case 0:
		__my_mpu6050->gyro_change_unit_factor = 131;
		break;

	case 1:
		__my_mpu6050->gyro_change_unit_factor = 65.5;
		break;

	case 2:
		__my_mpu6050->gyro_change_unit_factor = 32.8;
		break;

	case 3:
		__my_mpu6050->gyro_change_unit_factor = 16.4;
		break;

	default:
		break;
	}

	switch (accel_range_you_want) {
	case 0:
		__my_mpu6050->accel_change_unit_factor = 16384;
		break;

	case 1:
		__my_mpu6050->accel_change_unit_factor = 8192;
		break;

	case 2:
		__my_mpu6050->accel_change_unit_factor = 4096;
		break;

	case 3:
		__my_mpu6050->accel_change_unit_factor = 2048;
		break;

	default:
		break;
	}

}

void read_gyro(I2C_HandleTypeDef *hi2c, mpu6050 *__my_mpu6050,
		int unit_you_want) {
	uint8_t slave_address = MPU6050_ADDRESS_AD0_LOW << 1;
	uint8_t register_to_access = MPU6050_RA_GYRO_XOUT_H;
	uint8_t data_to_read[6];

	HAL_I2C_Master_Transmit(hi2c, slave_address, &register_to_access, 1, 1000);

	HAL_I2C_Master_Receive(hi2c, slave_address, data_to_read, 6, 1000);

	switch (unit_you_want) {
	case 1:
		__my_mpu6050->gy_x = (int16_t) (data_to_read[0] << 8 | data_to_read[1]);
		__my_mpu6050->gy_y = (int16_t) (data_to_read[2] << 8 | data_to_read[3]);
		__my_mpu6050->gy_z = (int16_t) (data_to_read[4] << 8 | data_to_read[5]);
		break;

	case 2:
		__my_mpu6050->gy_x_dps = (int16_t) (data_to_read[0] << 8
				| data_to_read[1]) / __my_mpu6050->gyro_change_unit_factor;
		__my_mpu6050->gy_y_dps = (int16_t) (data_to_read[2] << 8
				| data_to_read[3]) / __my_mpu6050->gyro_change_unit_factor;
		__my_mpu6050->gy_z_dps = (int16_t) (data_to_read[4] << 8
				| data_to_read[5]) / __my_mpu6050->gyro_change_unit_factor;
		break;

	default:
		break;
	}
}

void read_accel(I2C_HandleTypeDef *hi2c, mpu6050 *__my_mpu6050,
		int unit_you_want) {
	uint8_t slave_address = MPU6050_ADDRESS_AD0_LOW << 1;
	uint8_t register_to_access = MPU6050_RA_ACCEL_XOUT_H;
	uint8_t data_to_read[6];

	HAL_I2C_Master_Transmit(hi2c, slave_address, &register_to_access, 1, 1000);

	HAL_I2C_Master_Receive(hi2c, slave_address, data_to_read, 6, 1000);

	switch (unit_you_want) {
	case 0: //�ٷ� ����
		__my_mpu6050->ac_x = data_to_read[0] << 8 | data_to_read[1];
		__my_mpu6050->ac_y = data_to_read[2] << 8 | data_to_read[3];
		__my_mpu6050->ac_z = data_to_read[4] << 8 | data_to_read[5];
		break;

	case 2:
		__my_mpu6050->ac_x_g =
				(int16_t) (data_to_read[0] << 8 | data_to_read[1])
						/ __my_mpu6050->accel_change_unit_factor;
		__my_mpu6050->ac_y_g =
				(int16_t) (data_to_read[2] << 8 | data_to_read[3])
						/ __my_mpu6050->accel_change_unit_factor;
		__my_mpu6050->ac_z_g =
				(int16_t) (data_to_read[4] << 8 | data_to_read[5])
						/ __my_mpu6050->accel_change_unit_factor;
		break;

	default:
		break;
	}
}

uint8_t get_angle(mpu6050 *Mpu6050) {
	static uint8_t angle = 0;
	read_accel(&hi2c1, Mpu6050, 0);
	if (Mpu6050->ac_z < 1000) {
		if (Mpu6050->ac_x < -14000)
			angle = 0;
		else if (Mpu6050->ac_y > 14000)
			angle = 1;
		else if (Mpu6050->ac_x > 14000)
			angle = 2;
		else if (Mpu6050->ac_y < -14000)
			angle = 3;
	}
	return angle;
}
