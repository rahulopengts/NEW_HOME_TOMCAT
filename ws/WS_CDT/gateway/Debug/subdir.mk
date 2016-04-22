################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../MyTest.c \
../cmc.c \
../gw.c \
../strprocessor.c 

OBJS += \
./MyTest.o \
./cmc.o \
./gw.o \
./strprocessor.o 

C_DEPS += \
./MyTest.d \
./cmc.d \
./gw.d \
./strprocessor.d 


# Each subdirectory must supply rules for building sources it contributes
%.o: ../%.c
	@echo 'Building file: $<'
	@echo 'Invoking: Cygwin C Compiler'
	gcc -I"D:\cygwin64\usr\include" -I"D:\Home_Auto\NEW_HOME_TOMCAT\ws\WS_CDT\gateway" -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


