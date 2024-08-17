# Creates a pico-sdk subdirectory in our project for the libraries
pico_sdk_init()

# Tell CMake where to find the executable source file
add_executable(${PROJECT_NAME} 
    ${CMAKE_CURRENT_SOURCE_DIR}/main.c
)

# Link to pico_stdlib (gpio, time, etc. functions)
target_link_libraries(${PROJECT_NAME} 
    pico_stdlib
    hardware_pio
    FreeRTOS-Kernel-Heap4 # FreeRTOS kernel and dynamic heap
)


# Add subdirectories
add_subdirectory(sys)
add_subdirectory(hardware)
add_subdirectory(utils)
add_subdirectory(tasks)

# Create map/bin/hex/uf2 files
pico_add_extra_outputs(${PROJECT_NAME})

target_include_directories(${PROJECT_NAME} PRIVATE
        ${CMAKE_CURRENT_LIST_DIR}
)
