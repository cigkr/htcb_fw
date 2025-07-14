################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (13.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Core/User/ap.c \
../Core/User/bsp.c \
../Core/User/eeprom.c 

OBJS += \
./Core/User/ap.o \
./Core/User/bsp.o \
./Core/User/eeprom.o 

C_DEPS += \
./Core/User/ap.d \
./Core/User/bsp.d \
./Core/User/eeprom.d 


# Each subdirectory must supply rules for building sources it contributes
Core/User/%.o Core/User/%.su Core/User/%.cyclo: ../Core/User/%.c Core/User/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m3 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32F103xE -c -I../Core/Inc -I../Drivers/STM32F1xx_HAL_Driver/Inc/Legacy -I../Drivers/STM32F1xx_HAL_Driver/Inc -I../Drivers/CMSIS/Device/ST/STM32F1xx/Include -I../Drivers/CMSIS/Include -I../Middlewares/Third_Party/FreeRTOS/Source/include -I../Middlewares/Third_Party/FreeRTOS/Source/CMSIS_RTOS_V2 -I../Middlewares/Third_Party/FreeRTOS/Source/portable/GCC/ARM_CM3 -I"D:/Work/HaramTech/New Control Board/HTCB_STM32/htcb_fw/Core/User" -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfloat-abi=soft -mthumb -o "$@"

clean: clean-Core-2f-User

clean-Core-2f-User:
	-$(RM) ./Core/User/ap.cyclo ./Core/User/ap.d ./Core/User/ap.o ./Core/User/ap.su ./Core/User/bsp.cyclo ./Core/User/bsp.d ./Core/User/bsp.o ./Core/User/bsp.su ./Core/User/eeprom.cyclo ./Core/User/eeprom.d ./Core/User/eeprom.o ./Core/User/eeprom.su

.PHONY: clean-Core-2f-User

