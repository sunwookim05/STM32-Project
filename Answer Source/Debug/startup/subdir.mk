################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (10.3-2021.10)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
S_SRCS += \
../startup/startup_stm32l052xx.s 

OBJS += \
./startup/startup_stm32l052xx.o 

S_DEPS += \
./startup/startup_stm32l052xx.d 


# Each subdirectory must supply rules for building sources it contributes
startup/%.o: ../startup/%.s startup/subdir.mk
	arm-none-eabi-gcc -c -mcpu=cortex-m0plus -g3 -c -Wa,-W -x assembler-with-cpp -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfloat-abi=soft -mthumb -o "$@" "$<"

clean: clean-startup

clean-startup:
	-$(RM) ./startup/startup_stm32l052xx.d ./startup/startup_stm32l052xx.o

.PHONY: clean-startup

