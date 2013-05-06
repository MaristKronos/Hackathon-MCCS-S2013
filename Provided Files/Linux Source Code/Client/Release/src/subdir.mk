################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../src/GameControl.cpp \
../src/MSControl_Client.cpp \
../src/NetConversation.cpp \
../src/NetSupport.cpp \
../src/stdafx.cpp 

OBJS += \
./src/GameControl.o \
./src/MSControl_Client.o \
./src/NetConversation.o \
./src/NetSupport.o \
./src/stdafx.o 

CPP_DEPS += \
./src/GameControl.d \
./src/MSControl_Client.d \
./src/NetConversation.d \
./src/NetSupport.d \
./src/stdafx.d 


# Each subdirectory must supply rules for building sources it contributes
src/%.o: ../src/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: Cross G++ Compiler'
	g++ -O3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


