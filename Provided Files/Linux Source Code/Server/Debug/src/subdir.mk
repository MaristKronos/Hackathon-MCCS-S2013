################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../src/GameControl.cpp \
../src/MSControl_Server.cpp \
../src/NetConversation_Server.cpp \
../src/NetSupport_Server.cpp \
../src/VectorMatrix.cpp \
../src/stdafx.cpp 

OBJS += \
./src/GameControl.o \
./src/MSControl_Server.o \
./src/NetConversation_Server.o \
./src/NetSupport_Server.o \
./src/VectorMatrix.o \
./src/stdafx.o 

CPP_DEPS += \
./src/GameControl.d \
./src/MSControl_Server.d \
./src/NetConversation_Server.d \
./src/NetSupport_Server.d \
./src/VectorMatrix.d \
./src/stdafx.d 


# Each subdirectory must supply rules for building sources it contributes
src/%.o: ../src/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


