################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../unit_tests/ut_Dataset.cpp \
../unit_tests/ut_Node.cpp \
../unit_tests/ut_main.cpp 

OBJS += \
./unit_tests/ut_Dataset.o \
./unit_tests/ut_Node.o \
./unit_tests/ut_main.o 

CPP_DEPS += \
./unit_tests/ut_Dataset.d \
./unit_tests/ut_Node.d \
./unit_tests/ut_main.d 


# Each subdirectory must supply rules for building sources it contributes
unit_tests/%.o: ../unit_tests/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: Cross G++ Compiler'
	g++ -I"/media/sf_Protected/Documents/Workspace/RF/inc" -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


