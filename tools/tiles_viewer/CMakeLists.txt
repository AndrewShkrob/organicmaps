project(tiles_viewer)

set(SRC main.cpp)

omim_add_executable(${PROJECT_NAME} ${SRC})
set_target_properties(${PROJECT_NAME} PROPERTIES AUTOMOC ON)
target_link_libraries(${PROJECT_NAME} PRIVATE
  om::tiles
  Qt6::Widgets
  Qt6::Network
)
