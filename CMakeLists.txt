# Set minimum required version of CMake
cmake_minimum_required(VERSION 3.13)

set(PICO_DEOPTIMIZED_DEBUG 1)

# Include build functions from Pico SDK
include($ENV{PICO_SDK_PATH}/external/pico_sdk_import.cmake)

# Include FreeRTOS
if (FREERTOS_KERNEL_PATH OR DEFINED ENV{FREERTOS_KERNEL_PATH})
    include(FreeRTOS_Kernel_import.cmake)
endif()

# Set name of project (as PROJECT_NAME) and C/C   standards
project(dupico C CXX ASM)
set(CMAKE_C_STANDARD 17)
set(CMAKE_CXX_STANDARD 20)

add_compile_definitions(FW_VERSION="0.2.1")
add_compile_definitions(HW_MODEL=3)

add_compile_definitions(USE_D_PRINTF=1)
add_compile_definitions("DEBUG=$<CONFIG:Debug>")

# Add subdirectories for project
add_subdirectory(src)

target_compile_definitions(${PROJECT_NAME} INTERFACE
    NO_SYS=0
    FREE_RTOS_KERNEL_SMP=1
)

# Enable usb output, disable uart output
pico_enable_stdio_usb(${PROJECT_NAME} 1)
pico_enable_stdio_uart(${PROJECT_NAME} 1)
