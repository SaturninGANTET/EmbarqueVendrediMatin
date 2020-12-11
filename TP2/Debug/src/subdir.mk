################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../src/TP2.c \
../src/tp2-algo.c 

OBJS += \
./src/TP2.o \
./src/tp2-algo.o 

C_DEPS += \
./src/TP2.d \
./src/tp2-algo.d 


# Each subdirectory must supply rules for building sources it contributes
src/%.o: ../src/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: Cross GCC Compiler'
	sparc-gaisler-elf-gcc -O3 -g3 -Wall -c -fmessage-length=0 -O2 -g3 -qbsp=leon3 -msoft-float -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


