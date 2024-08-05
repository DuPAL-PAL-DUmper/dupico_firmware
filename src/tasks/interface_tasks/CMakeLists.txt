add_subdirectory(cmd_handlers)

target_sources(${PROJECT_NAME}
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/cli_interface_task.c
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/cli_interface_task.h
)