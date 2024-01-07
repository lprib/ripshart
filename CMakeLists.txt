cmake_minimum_required(VERSION 3.20.0 FATAL_ERROR)

project(ripshart)

set(RAYLIB_STATIC_LIB_LOCATION /usr/local/lib/libraylib.a CACHE FILEPATH "Where to find libraylib.a")
set(RAYLIB_HEADER_FOLDER /usr/local/include CACHE PATH "Where to find raylib/rlgl/raymath headers")

set(CMAKE_CXX_STANDARD 20)

add_library(libtubemap
    libtubemap/tube_file.cpp
    libtubemap/tube_file.hpp
    libtubemap/tube_map.hpp
    libtubemap/tube_render.cpp
    libtubemap/tube_render.hpp
)
target_link_libraries(libtubemap ${RAYLIB_STATIC_LIB_LOCATION})
target_include_directories(libtubemap PUBLIC ${RAYLIB_HEADER_FOLDER})
target_include_directories(libtubemap PUBLIC libtubemap)

add_executable(editor
    editor/main.cpp
)
target_link_libraries(editor
    ${RAYLIB_STATIC_LIB_LOCATION}
    libtubemap
)
target_include_directories(editor PRIVATE ${RAYLIB_HEADER_FOLDER})
