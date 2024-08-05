target_sources(${PROJECT_NAME}
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/bin_cmd_handler.c
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/cmd_handler.h
        ${CMAKE_CURRENT_SOURCE_DIR}/bin_cmd_handler.h
)