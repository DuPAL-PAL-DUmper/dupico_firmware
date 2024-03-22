# Add subdirectories
add_subdirectory(ssd1306_i2c)
add_subdirectory(sdcard)

target_include_directories(${PROJECT_NAME} PRIVATE
        ${CMAKE_CURRENT_LIST_DIR}
        ${CMAKE_CURRENT_SOURCE_DIR}
)