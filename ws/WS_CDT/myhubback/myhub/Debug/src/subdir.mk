################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../src/Clients.c \
../src/Heap.c \
../src/LinkedList.c \
../src/Log.c \
../src/MQTTAsync.c \
../src/MQTTAsync_publish.c \
../src/MQTTPacket.c \
../src/MQTTPacketOut.c \
../src/MQTTPersistence.c \
../src/MQTTPersistenceDefault.c \
../src/MQTTProtocolClient.c \
../src/MQTTProtocolOut.c \
../src/MQTTVersion.c \
../src/Messages.c \
../src/SSLSocket.c \
../src/Socket.c \
../src/SocketBuffer.c \
../src/StackTrace.c \
../src/Thread.c \
../src/Tree.c \
../src/myhub.c \
../src/utf-8.c 

OBJS += \
./src/Clients.o \
./src/Heap.o \
./src/LinkedList.o \
./src/Log.o \
./src/MQTTAsync.o \
./src/MQTTAsync_publish.o \
./src/MQTTPacket.o \
./src/MQTTPacketOut.o \
./src/MQTTPersistence.o \
./src/MQTTPersistenceDefault.o \
./src/MQTTProtocolClient.o \
./src/MQTTProtocolOut.o \
./src/MQTTVersion.o \
./src/Messages.o \
./src/SSLSocket.o \
./src/Socket.o \
./src/SocketBuffer.o \
./src/StackTrace.o \
./src/Thread.o \
./src/Tree.o \
./src/myhub.o \
./src/utf-8.o 

C_DEPS += \
./src/Clients.d \
./src/Heap.d \
./src/LinkedList.d \
./src/Log.d \
./src/MQTTAsync.d \
./src/MQTTAsync_publish.d \
./src/MQTTPacket.d \
./src/MQTTPacketOut.d \
./src/MQTTPersistence.d \
./src/MQTTPersistenceDefault.d \
./src/MQTTProtocolClient.d \
./src/MQTTProtocolOut.d \
./src/MQTTVersion.d \
./src/Messages.d \
./src/SSLSocket.d \
./src/Socket.d \
./src/SocketBuffer.d \
./src/StackTrace.d \
./src/Thread.d \
./src/Tree.d \
./src/myhub.d \
./src/utf-8.d 


# Each subdirectory must supply rules for building sources it contributes
src/%.o: ../src/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: Cygwin C Compiler'
	gcc -I"D:\cygwin64\lib\gcc\x86_64-pc-cygwin\5.3.0\include" -I"D:\cygwin64\usr\include" -I"D:\cygwin64\usr\include\w32api" -I"D:\Home_Auto\NEW_HOME_TOMCAT\ws\WS_CDT\myhub\src\include" -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


