cmake_minimum_required(VERSION 3.15)
project(myFuncPack)

set(CMAKE_CXX_STANDARD 14)

add_executable(mymv mymv.cpp utils.h utils.cpp)