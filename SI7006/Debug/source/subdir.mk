################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../source/Task2_SI7006.c \
../source/mtb.c \
../source/semihost_hardfault.c 

OBJS += \
./source/Task2_SI7006.o \
./source/mtb.o \
./source/semihost_hardfault.o 

C_DEPS += \
./source/Task2_SI7006.d \
./source/mtb.d \
./source/semihost_hardfault.d 


# Each subdirectory must supply rules for building sources it contributes
source/%.o: ../source/%.c source/subdir.mk
	@echo 'Building file: $<'
	@echo 'Invoking: MCU C Compiler'
	arm-none-eabi-gcc -DCPU_MKL25Z128VLK4_cm0plus -DCPU_MKL25Z128VLK4 -DFSL_RTOS_BM -DSDK_OS_BAREMETAL -DSDK_DEBUGCONSOLE=0 -DCR_INTEGER_PRINTF -DPRINTF_FLOAT_ENABLE=0 -D__MCUXPRESSO -D__USE_CMSIS -DDEBUG -D__REDLIB__ -I"C:\Users\Pravin\Documents\MCUXpressoIDE_11.5.1_7266\workspace\Task2_SI7006\board" -I"C:\Users\Pravin\Documents\MCUXpressoIDE_11.5.1_7266\workspace\Task2_SI7006\source" -I"C:\Users\Pravin\Documents\MCUXpressoIDE_11.5.1_7266\workspace\Task2_SI7006" -I"C:\Users\Pravin\Documents\MCUXpressoIDE_11.5.1_7266\workspace\Task2_SI7006\startup" -I"C:\Users\Pravin\Documents\MCUXpressoIDE_11.5.1_7266\workspace\Task2_SI7006\CMSIS" -I"C:\Users\Pravin\Documents\MCUXpressoIDE_11.5.1_7266\workspace\Task2_SI7006\drivers" -I"C:\Users\Pravin\Documents\MCUXpressoIDE_11.5.1_7266\workspace\Task2_SI7006\utilities" -O0 -fno-common -g3 -Wall -c -fmessage-length=0 -fno-builtin -ffunction-sections -fdata-sections -fmerge-constants -fmacro-prefix-map="$(<D)/"= -mcpu=cortex-m0plus -mthumb -D__REDLIB__ -fstack-usage -specs=redlib.specs -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.o)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


