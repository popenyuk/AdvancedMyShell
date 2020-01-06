#ifndef MYGREP_ARG_READER_H
#define MYGREP_ARG_READER_H

#include <string>

struct arg_reader{
    bool help{false};
    bool invert_match{false};
    bool ignore_case{false};
    std::string filename;
    std::string regexpr;
} typedef arg_reader;

void parse_arguments(int argc, char *argv[], arg_reader& my_reader);
#endif //MYGREP_ARG_READER_H
