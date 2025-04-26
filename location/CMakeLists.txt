project(location)

set(PUBLIC_INCLUDES
    ${CMAKE_CURRENT_SOURCE_DIR}/include/
)

set(SRC
    ${CMAKE_CURRENT_SOURCE_DIR}/src/om/location/location_manager.cpp
)

omim_add_library(${PROJECT_NAME} ${PUBLIC_INCLUDES} ${SRC})

target_include_directories(${PROJECT_NAME} PUBLIC
    ${PUBLIC_INCLUDES}
)
