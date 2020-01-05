// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: http://www.viva64.com

#ifndef MYLS_ARGUMENTS_READER_H
#define MYLS_ARGUMENTS_READER_H

#include <string>
#include <vector>

struct arg_reader{
    bool help{false};
    bool recursive{false};
    bool full_information{false};
    bool f{false};
    bool r{false};
    bool first_dir{false};
    std::vector<std::string> pathes;
    std::string sort;

} typedef arg_reader;

void read_arguments(int argc, char *argv[], arg_reader& my_reader);
#endif //MYLS_ARGUMENTS_READER_H
