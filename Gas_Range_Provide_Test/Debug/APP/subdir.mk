################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (10.3-2021.10)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../APP/app.c \
../APP/led_ring.c 

OBJS += \
./APP/app.o \
./APP/led_ring.o 

C_DEPS += \
./APP/app.d \
./APP/led_ring.d 


# Each subdirectory must supply rules for building sources it contributes
APP/%.o APP/%.su: ../APP/%.c APP/subdir.mk
	arm-none-eabi-gcc -c "$<" -mcpu=cortex-m0plus -std=gnu11 -g3 -DUSE_HAL_DRIVER -DSTM32L052xx -c -I../Inc -I../Drivers/STM32L0xx_HAL_Driver/Inc -I../Drivers/STM32L0xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32L0xx/Include -I../Drivers/CMSIS/Include -I../APP -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfloat-abi=soft -mthumb -o "$@"

clean: clean-APP

clean-APP:
	-$(RM) ./APP/app.d ./APP/app.o ./APP/app.su ./APP/led_ring.d ./APP/led_ring.o ./APP/led_ring.su

.PHONY: clean-APP

