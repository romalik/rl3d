  

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "./cmake")

find_package(SDL2 REQUIRED)
include_directories(${SDL2_INCLUDE_DIR})
add_executable( sdl3d sdl3d.cpp)
target_link_libraries( sdl3d ${SDL2_LIBRARY} )
