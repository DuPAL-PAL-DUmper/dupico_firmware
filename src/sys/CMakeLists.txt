target_sources(${PROJECT_NAME}
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/sys.c
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/sys.h
)

#target_include_directories(${PROJECT_NAME} PRIVATE
#        ${CMAKE_CURRENT_LIST_DIR}
#        ${CMAKE_CURRENT_SOURCE_DIR}
#)