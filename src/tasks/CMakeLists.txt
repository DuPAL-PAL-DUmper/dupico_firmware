add_subdirectory(ic_handlers)
add_subdirectory(interface_tasks)

target_sources(${PROJECT_NAME}
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/shifter_io_task.c
        ${CMAKE_CURRENT_SOURCE_DIR}/ic_interfacer_task.c
        ${CMAKE_CURRENT_SOURCE_DIR}/ic_command_executor_task.c
        ${CMAKE_CURRENT_SOURCE_DIR}/command_hub_task.c
        ${CMAKE_CURRENT_SOURCE_DIR}/data_structs.c
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/shifter_io_task.h
        ${CMAKE_CURRENT_SOURCE_DIR}/ic_interfacer_task.h
        ${CMAKE_CURRENT_SOURCE_DIR}/ic_command_executor_task.h
        ${CMAKE_CURRENT_SOURCE_DIR}/command_hub_task.h
        ${CMAKE_CURRENT_SOURCE_DIR}/data_structs.h
)