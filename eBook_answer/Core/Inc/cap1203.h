/*
 * cap1203.h
 *
 *  Created on: 2022. 1. 24.
 *      Author: tls123
 */

#ifndef __CAP1203_H_
#define __CAP1203_H_
#include "stdint.h"
#define CAP1203_I2C_ADDR 0x28

// Register values

// Product ID - always the same (pg. 22)
#define PROD_ID_VALUE 0x6D

// Capacitive sensor input (pg. 23)
#define OFF 0x00 // No touch detecetd
#define ON 0x01  // Check capacitive sensor input (pg. 23)

// Pads to be power buttion (pg. 43)
#define PWR_CS1 0x00 // Pad 1 (Left)
#define PWR_CS2 0x01 // Pad 2 (Middle)
#define PWR_CS3 0x02 // Pad 3 (Right)

#define PAD_LEFT PWR_CS1
#define PAD_MIDDLE PWR_CS2
#define PAD_RIGHT PWR_CS3

// Power button hold time to generate interrupt (pg. 44)
#define PWR_TIME_280_MS 0x00  // 280 ms
#define PWR_TIME_560_MS 0x01  // 560 ms
#define PWR_TIME_1120_MS 0x02 // 1.12 sec
#define PWR_TIME_2240_MS 0x03 // 2.24 sec

// Sensitivity for touch detection (pg. 25)
#define SENSITIVITY_128X 0x00 // Most sensitive
#define SENSITIVITY_64X 0x01
#define SENSITIVITY_32X 0x02
#define SENSITIVITY_16X 0x03
#define SENSITIVITY_8X 0x04
#define SENSITIVITY_4X 0x05
#define SENSITIVITY_2X 0x06
#define SENSITIVITY_1X 0x07

enum CAP1203_Register
{
    MAIN_CONTROL = 0x00,
    GENERAL_STATUS = 0x02,
    SENSOR_INPUT_STATUS = 0x03,
    NOISE_FLAG_STATUS = 0x0A,
    SENSOR_INPUT_1_DELTA_COUNT = 0x10,
    SENSOR_INPUT_2_DELTA_COUNT = 0X11,
    SENSOR_INPUT_3_DELTA_COUNT = 0X12,
    SENSITIVITY_CONTROL = 0x1F,
    CONFIG = 0x20,
    SENSOR_INPUT_ENABLE = 0x21,
    SENSOR_INPUT_CONFIG = 0x22,
    SENSOR_INPUT_CONFIG_2 = 0x23,
    AVERAGING_AND_SAMPLE_CONFIG = 0x24,
    CALIBRATION_ACTIVATE_AND_STATUS = 0x26,
    INTERRUPT_ENABLE = 0x27,
    REPEAT_RATE_ENABLE = 0x28,
    MULTIPLE_TOUCH_CONFIG = 0x2A,
    MULTIPLE_TOUCH_PATTERN_CONFIG = 0x2B,
    MULTIPLE_TOUCH_PATTERN = 0x2D,
    BASE_COUNT_OUT = 0x2E,
    RECALIBRATION_CONFIG = 0x2F,
    SENSOR_1_INPUT_THRESH = 0x30,
    SENSOR_2_INPUT_THRESH = 0x31,
    SENSOR_3_INPUT_THRESH = 0x32,
    SENSOR_INPUT_NOISE_THRESH = 0x38,
    STANDBY_CHANNEL = 0x40,
    STANDBY_CONFIG = 0x41,
    STANDBY_SENSITIVITY = 0x42,
    STANDBY_THRESH = 0x43,
    CONFIG_2 = 0x44,
    SENSOR_INPUT_1_BASE_COUNT = 0x50,
    SENSOR_INPUT_2_BASE_COUNT = 0x51,
    SENSOR_INPUT_3_BASE_COUNT = 0x52,
    POWER_BUTTON = 0x60,
    POWER_BUTTON_CONFIG = 0x61,
    SENSOR_INPUT_1_CALIBRATION = 0xB1,
    SENSOR_INPUT_2_CALIBRATION = 0xB2,
    SENSOR_INPUT_3_CALIBRATION = 0xB3,
    SENSOR_INPUT_CALIBRATION_LSB_1 = 0xB9,
    PROD_ID = 0xFD,
    MANUFACTURE_ID = 0xFE,
    REVISION = 0xFF,
};

void cap1203_init(uint8_t sensitivity); //°¨µµ
uint8_t cap1203_getinputvalue(uint8_t *temp);
void setSensitivity(uint8_t sensitivity);

#endif /* INC_CAP1203_H_ */
