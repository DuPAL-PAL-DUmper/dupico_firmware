add_subdirectory(interface_tasks)

target_sources(${PROJECT_NAME}
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/shifter_io_task.c
        ${CMAKE_CURRENT_SOURCE_DIR}/command_hub_task.c
        ${CMAKE_CURRENT_SOURCE_DIR}/led_status_task.c
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/shifter_io_task.h
        ${CMAKE_CURRENT_SOURCE_DIR}/command_hub_task.h
        ${CMAKE_CURRENT_SOURCE_DIR}/led_status_task.h
)