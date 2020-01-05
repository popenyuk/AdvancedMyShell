// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: http://www.viva64.com

#include "argument_manager.h"

void parse_arguments(int argc, char **argv, arg_reader &my_reader) {
    for(size_t index = 1; index < argc; ++index){
        std::string arg{argv[index]};
        if(arg == "-h" || arg == "--help"){
            my_reader.help = true;
        }else if(arg == "-p"){
            my_reader.p = true;
        }else{
            my_reader.directory_name = arg;
        }
    }
}
