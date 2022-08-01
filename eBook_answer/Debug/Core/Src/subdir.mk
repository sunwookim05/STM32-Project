################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (9-2020-q2-update)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Core/Src/MPU6050.c \
../Core/Src/cap1203.c \
../Core/Src/fonts.c \
../Core/Src/main.c \
../Core/Src/ssd1306.c \
../Core/Src/stm32l0xx_hal_msp.c \
../Core/Src/stm32l0xx_it.c \
../Core/Src/syscalls.c \
../Core/Src/sysmem.c \
../Core/Src/system_stm32l0xx.c 

OBJS += \
./Core/Src/MPU6050.o \
./Core/Src/cap1203.o \
./Core/Src/fonts.o \
./Core/Src/main.o \
./Core/Src/ssd1306.o \
./Core/Src/stm32l0xx_hal_msp.o \
./Core/Src/stm32l0xx_it.o \
./Core/Src/syscalls.o \
./Core/Src/sysmem.o \
./Core/Src/system_stm32l0xx.o 

C_DEPS += \
./Core/Src/MPU6050.d \
./Core/Src/cap1203.d \
./Core/Src/fonts.d \
./Core/Src/main.d \
./Core/Src/ssd1306.d \
./Core/Src/stm32l0xx_hal_msp.d \
./Core/Src/stm32l0xx_it.d \
./Core/Src/syscalls.d \
./Core/Src/sysmem.d \
./Core/Src/system_stm32l0xx.d 


# Each subdirectory must supply rules for building sources it contributes
Core/Src/%.o: ../Core/Src/%.c Core/Src/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m0plus -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32L052xx -c -I../FATFS/Target -I../FATFS/App -I../Core/Inc -I../Drivers/STM32L0xx_HAL_Driver/Inc -I../Drivers/STM32L0xx_HAL_Driver/Inc/Legacy -I../Middlewares/Third_Party/FatFs/src -I../Drivers/CMSIS/Device/ST/STM32L0xx/Include -I../Drivers/CMSIS/Include -Os -ffunction-sections -fdata-sections -Wall -fstack-usage -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfloat-abi=soft -mthumb -o "$@"

clean: clean-Core-2f-Src

clean-Core-2f-Src:
	-$(RM) ./Core/Src/MPU6050.d ./Core/Src/MPU6050.o ./Core/Src/cap1203.d ./Core/Src/cap1203.o ./Core/Src/fonts.d ./Core/Src/fonts.o ./Core/Src/main.d ./Core/Src/main.o ./Core/Src/ssd1306.d ./Core/Src/ssd1306.o ./Core/Src/stm32l0xx_hal_msp.d ./Core/Src/stm32l0xx_hal_msp.o ./Core/Src/stm32l0xx_it.d ./Core/Src/stm32l0xx_it.o ./Core/Src/syscalls.d ./Core/Src/syscalls.o ./Core/Src/sysmem.d ./Core/Src/sysmem.o ./Core/Src/system_stm32l0xx.d ./Core/Src/system_stm32l0xx.o

.PHONY: clean-Core-2f-Src

