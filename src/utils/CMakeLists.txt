target_include_directories(${PROJECT_NAME} PRIVATE
        ${CMAKE_CURRENT_LIST_DIR}
        ${CMAKE_CURRENT_SOURCE_DIR}
)

target_sources(${PROJECT_NAME}
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/common_macros.h
        ${CMAKE_CURRENT_SOURCE_DIR}/custom_debug.h
        ${CMAKE_CURRENT_SOURCE_DIR}/strutils.h
        ${CMAKE_CURRENT_SOURCE_DIR}/binutils.h
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/custom_debug.c
        ${CMAKE_CURRENT_SOURCE_DIR}/strutils.c
        ${CMAKE_CURRENT_SOURCE_DIR}/binutils.c
)