target_sources(${PROJECT_NAME}
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/piso_shifter.c
        ${CMAKE_CURRENT_SOURCE_DIR}/sipo_shifter.c
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/piso_shifter.h
        ${CMAKE_CURRENT_SOURCE_DIR}/sipo_shifter.h
)

# Create C header file with the name <pio program>.pio.h
pico_generate_pio_header(${PROJECT_NAME}  
        ${CMAKE_CURRENT_LIST_DIR}/piso.pio
)

pico_generate_pio_header(${PROJECT_NAME}  
        ${CMAKE_CURRENT_LIST_DIR}/sipo.pio
)