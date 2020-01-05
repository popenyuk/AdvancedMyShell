cmake_minimum_required(VERSION 3.10)
project(myls)

include_directories(sources headers)
set(CMAKE_CXX_STANDARD 14)

add_executable(myls main.cpp sources/File.cpp headers/File.h sources/FilesManagement.cpp headers/FilesManagement.h
        sources/arguments_reader.cpp headers/arguments_reader.h headers/wildcard_parser.h sources/wildcard_parser.cpp)