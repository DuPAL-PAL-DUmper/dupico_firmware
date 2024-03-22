target_sources(${PROJECT_NAME}
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/piso_shifter.c
        ${CMAKE_CURRENT_SOURCE_DIR}/sipo_shifter.c
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/piso_shifter.h
        ${CMAKE_CURRENT_SOURCE_DIR}/sipo_shifter.h
)