project(tiles)

set(SRC
  cache.hpp
  configs.cpp
  configs.hpp
  server_config.hpp
  server_thread.hpp
  tile.hpp
  tile_request_builder.cpp
  tile_request_builder.hpp
  tile_server.cpp
  tile_server.hpp
)

omim_add_library(${PROJECT_NAME} ${SRC})
add_library(om::tiles ALIAS ${PROJECT_NAME})

target_link_libraries(${PROJECT_NAME} PUBLIC base platform stb_image drape_frontend)
