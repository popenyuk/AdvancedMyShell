// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: http://www.viva64.com

#ifndef MYMKDIR_ARGUMENT_MANAGER_H
#define MYMKDIR_ARGUMENT_MANAGER_H

#include <string>
#include <vector>

struct arg_reader{
    bool help{false};
    bool force{false};
    bool recursive{false};
    std::vector<std::string> files;

} typedef arg_reader;

void parse_arguments(int argc, char *argv[], arg_reader& my_reader);

#endif //MYMKDIR_ARGUMENT_MANAGER_H
