// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: http://www.viva64.com

#include <iostream>
#include <algorithm>
#include "arguments_reader.h"

void read_arguments(int argc, char *argv[], arg_reader &my_reader) {
    bool double_hyphen{false};
    for (size_t index = 1; index < argc; ++index) {
        std::string arg{*(argv + index)};
        if (double_hyphen || arg.find('-') != 0) {
            if (index == 1) {
                my_reader.first_dir = true;
            }
            if (arg.find('"') == 0) {
                std::cout << arg.substr(arg.find('"') + 1, arg.length() - 1) << std::endl;
                my_reader.pathes.push_back(arg.substr(arg.find('"') + 1, arg.length() - 1));
            } else {
                my_reader.pathes.push_back(arg);
            }
        } else if (arg == "-h" || arg == "--help") {
            my_reader.help = true;
        } else if (arg == "-l") {
            my_reader.full_information = true;
        } else if (arg == "-r") {
            my_reader.r = true;
        } else if (arg == "-F") {
            my_reader.f = true;
        } else if (arg == "-R") {
            my_reader.recursive = true;
        } else if (arg.find("--sort=") == 0) {
            my_reader.sort = arg.substr(arg.find("--sort=") + 7);
        } else if (arg == "--") {
            double_hyphen = true;
        } else {
            std::cerr << "Unknown option " << arg << "." << std::endl;
            exit(-1);
        }
    }
}
