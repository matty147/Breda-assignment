cmake_minimum_required(VERSION 3.20)
project(MyGame)

set(CMAKE_CXX_STANDARD 20)


add_executable(MyGame
        game.cpp
        player.cpp
        level.cpp
        tmpl8/surface.cpp
        tmpl8/template.cpp
)

file(COPY assets DESTINATION ${CMAKE_BINARY_DIR})

include_directories(tmpl8)
include_directories(lib/FreeImage/inc)
include_directories(lib/SDL2-2.0.3/include)

target_link_libraries(MyGame
        "${PROJECT_SOURCE_DIR}/lib/SDL2-2.0.3/lib/x64/SDL2.lib"
        "${PROJECT_SOURCE_DIR}/lib/SDL2-2.0.3/lib/x64/SDL2main.lib"
        "${PROJECT_SOURCE_DIR}/lib/FreeImage/lib64/FreeImage.lib"
)

