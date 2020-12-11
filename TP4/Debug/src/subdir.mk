################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../src/TP4.c \
../src/buffer_circulaire-partagee.c \
../src/buffer_circulaire.c 

OBJS += \
./src/TP4.o \
./src/buffer_circulaire-partagee.o \
./src/buffer_circulaire.o 

C_DEPS += \
./src/TP4.d \
./src/buffer_circulaire-partagee.d \
./src/buffer_circulaire.d 


# Each subdirectory must supply rules for building sources it contributes
src/%.o: ../src/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: Cross GCC Compiler'
	sparc-gaisler-rtems5-gcc -I"/home/satutu/eclipse-workspace/TP4/windows-producer" -O0 -g3 -Wall -c -fmessage-length=0 -qbsp=gr712rc -msoft-float -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


