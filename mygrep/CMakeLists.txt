cmake_minimum_required(VERSION 3.10)
project(mygrep)

include_directories(src inc)
set(CMAKE_CXX_STANDARD 11)

add_executable(mygrep main.cpp src/arg_reader.cpp inc/arg_reader.h src/myprint.cpp inc/myprint.h)