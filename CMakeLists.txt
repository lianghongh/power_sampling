cmake_minimum_required(VERSION 3.8)
project(power_sampling)

set(CMAKE_C_STANDARD 99)

set(SOURCE_FILES main.c msr_config.h msr_config.c sampling_data.h sampling_data.c temp.h temp.c)
add_executable(power_sampling ${SOURCE_FILES})