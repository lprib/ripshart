cmake_minimum_required(VERSION 3.20.0 FATAL_ERROR)

project(ripshart)

set(RAYLIB_STATIC_LIB_LOCATION /usr/local/lib/libraylib.a CACHE FILEPATH "Where to find libraylib.a")
set(RAYLIB_HEADER_FOLDER /usr/local/include CACHE PATH "Where to find raylib/rlgl/raymath headers")

set(CMAKE_CXX_STANDARD 20)

add_executable(ripshart_unshackled
    main.cpp
    line.cpp
    line.hpp
    train.cpp
    train.hpp
)

target_link_libraries(ripshart_unshackled ${RAYLIB_STATIC_LIB_LOCATION})
target_include_directories(ripshart_unshackled PRIVATE ${RAYLIB_HEADER_FOLDER})
