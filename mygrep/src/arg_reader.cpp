#include "arg_reader.h"

void parse_arguments(int argc, char **argv, arg_reader &my_reader) {
    for (size_t index = 1; index < argc; ++index) {
        std::string arg{argv[index]};
        if (arg == "-h" || arg == "--help") {
            my_reader.help = true;
        } else if (arg == "-v" || arg == "--invert-match") {
            my_reader.invert_match = true;
        } else if (arg == "-i" || arg == "--ignore-case") {
            my_reader.ignore_case = true;
        } else if (arg.find("--file=", 0) == 0) {
            my_reader.filename = arg.substr(7);
        } else if (arg.find("--regexp=", 0) == 0) {
            my_reader.regexpr = arg.substr(9);
        } else if(my_reader.regexpr.empty()){
            my_reader.regexpr = arg;
        }
    }
}
