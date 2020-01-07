// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: http://www.viva64.com

#include "args_manager.h"

void parse_arguments(int argc, char **argv, arg_reader &my_reader) {
    for(size_t index = 1; index < argc; ++index){
        std::string arg{argv[index]};
        if(arg == "-h" || arg == "--help"){
            my_reader.help = true;
        }else if(arg == "-f"){
            my_reader.force = true;
        }else if(arg == "-R"){
            my_reader.recursive = true;
        }else{
            my_reader.files.push_back(arg);
        }
    }
}
