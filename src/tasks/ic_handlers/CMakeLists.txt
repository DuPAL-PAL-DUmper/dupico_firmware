target_sources(${PROJECT_NAME}
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/ic_handlers.c
        ${CMAKE_CURRENT_SOURCE_DIR}/unknown_ic_handler.c
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/ic_handlers.h
        ${CMAKE_CURRENT_SOURCE_DIR}/unknown_ic_handler.h
)