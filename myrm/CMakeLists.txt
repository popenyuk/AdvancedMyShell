cmake_minimum_required(VERSION 3.10)
project(myrm CXX)

set(CMAKE_CXX_STANDARD 17)
link_libraries(stdc++fs)
include_directories(src inc)
add_executable(myrm main.cpp inc/argument_manager.h src/argument_manager.cpp)