cmake_minimum_required(VERSION 3.10)
project(mymkdir)

include_directories(Inc Src)
set(CMAKE_CXX_STANDARD 17)

add_executable(mymkdir main.cpp Src/argument_manager.cpp Inc/argument_manager.h Src/folder_creator.cpp Inc/folder_creator.h)