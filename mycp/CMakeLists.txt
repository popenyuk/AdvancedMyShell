cmake_minimum_required(VERSION 3.10)
project(myFuncPack)

set(CMAKE_CXX_STANDARD 14)

add_executable(mycp mycp.cpp utils.h utils.cpp)
