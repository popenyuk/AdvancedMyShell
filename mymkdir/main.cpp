// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: http://www.viva64.com

#include <iostream>
#include "folder_creator.h"
#include "argument_manager.h"

int main(int argc, char **argv) {
    arg_reader reader;
    parse_arguments(argc, argv, reader);
    if (reader.help) {
        std::cout << "Usage: mkdir [OPTION]... DIRECTORY\n"
                     "Create the DIRECTORY, if they do not already exist.\n"
                     "Mandatory arguments to long options are mandatory for short options too.\n"
                     "  -p, --parents     no error if existing, make parent directories as needed\n"
                     "  -h, --help     display this help and exit\n" << std::endl;
    } else {
        if (reader.p) {
            return create_folder_p(reader.directory_name);
        } else {
            return create_folder(reader.directory_name);
        }
    }
    return 0;
}
