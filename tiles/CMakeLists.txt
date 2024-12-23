project(tiles)

set(PUBLIC_HEADERS
  includes/om/tiles/configs.hpp
  includes/om/tiles/server_config.hpp
  includes/om/tiles/tile.hpp
  includes/om/tiles/tile_manager.hpp
  includes/om/tiles/tile_request_builder.hpp
  includes/om/tiles/tile_server.hpp
)

set(SRC
  src/om/tiles/server_thread.hpp
  src/om/tiles/tile_request_builder.cpp
  src/om/tiles/tile_server.cpp
)

omim_add_library(${PROJECT_NAME} ${PUBLIC_HEADERS})
add_library(om::tiles ALIAS ${PROJECT_NAME})
target_include_directories(${PROJECT_NAME} PUBLIC includes)
target_sources(${PROJECT_NAME} PRIVATE ${SRC})

target_link_libraries(${PROJECT_NAME} PUBLIC base platform)

omim_add_tool_subdirectory(tools/tile_viewer)
