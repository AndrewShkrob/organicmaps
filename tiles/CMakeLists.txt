project(tiles)

set(PUBLIC_INCLUDES
    ${CMAKE_CURRENT_SOURCE_DIR}/include/
)

set(PRIVATE_INCLUDES
    ${CMAKE_CURRENT_SOURCE_DIR}/src/
)

set(SRC
    src/om/tiles/provider/osm.cpp
    src/om/tiles/server/server.cpp
    src/om/tiles/tiles_manager.cpp
)

add_library(${PROJECT_NAME} STATIC ${PUBLIC_INCLUDES} ${PRIVATE_INCLUDES} ${SRC})

target_include_directories(${PROJECT_NAME}
    PUBLIC ${PUBLIC_INCLUDES}
    PRIVATE ${PRIVATE_INCLUDES}
)

target_link_libraries(${PROJECT_NAME}
    PUBLIC geometry
    PRIVATE base drape_frontend
)

add_library(om::${PROJECT_NAME} ALIAS ${PROJECT_NAME})
