cmake_minimum_required(VERSION 3.10)
project(myshell)

set(CMAKE_CXX_STANDARD 17)

find_library(Readline_LIBRARY NAMES readline)

include_directories(includes)
include_directories(mymkdir/Inc)
include_directories(myls/headers)
include_directories(mygrep/inc)

add_executable(mycat mycat/main.c mycat/my_io.c mycat/my_io.h)

add_executable(myls myls/main.cpp myls/sources/File.cpp myls/headers/File.h myls/sources/FilesManagement.cpp myls/headers/FilesManagement.h
        myls/sources/arguments_reader.cpp myls/headers/arguments_reader.h myls/headers/wildcard_parser.h myls/sources/wildcard_parser.cpp)

add_executable(mymkdir mymkdir/main.cpp mymkdir/Src/argument_manager.cpp mymkdir/Inc/argument_manager.h mymkdir/Src/folder_creator.cpp mymkdir/Inc/folder_creator.h)

add_executable(myshell src/main.cpp src/Input.cpp src/wildcard_parser.cpp src/merrno.cpp includes/merrno.h
        src/my_functions.cpp src/my_programs.cpp src/helpers.cpp src/VariablesManager.cpp src/my_io.cpp includes/my_io.h src/my_file_reader.cpp includes/my_file_reader.h)

add_executable(mygrep mygrep/main.cpp mygrep/src/arg_reader.cpp mygrep/inc/arg_reader.h mygrep/src/myprint.cpp mygrep/inc/myprint.h)

target_link_libraries(myshell readline)