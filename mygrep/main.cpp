#include <regex>
#include <iostream>
#include "myprint.h"
#include "arg_reader.h"

int main(int argc, char **argv) {
    arg_reader args;
    parse_arguments(argc, argv, args);
    if (argc == 1 || args.regexpr.empty()) {
        std::cout << "Wrong number of arguments." << std::endl;
        return -1;
    }
    std::regex regex;
    try {
        if(args.ignore_case){
            regex = std::regex("(" + args.regexpr + ")", std::regex_constants::icase);
        }else{
            regex = std::regex("(" + args.regexpr + ")");
        }
    } catch (std::regex_error &e) {
        std::cout << "You regular expression is bad!" << std::endl;
        return -1;
    }
    std::string input;
    if (args.help) {
        std::cout
                << "grep [-h|--help] [-v|--invert-match] [-i|--ignore-case] [--file=<filename>] [--regexp=’<regexpression>’ | string]\n"
                   "\tIf no argument is passed, it only leaves the strings where the string match occurred\n"
                   "\tIf -v is only where it did not occur.\n"
                   "\tIf -i - filter, ignoring case-sensitive letters.\n"
                   "\tIf there is an option --regexp - filter the tapes according to the regular expression, otherwise - according to string.\n"
                   "\tIf the command is successful, the end code must be null, not null in case of error." << std::endl;
    } else {
        while (std::getline(std::cin, input)) {
            std::smatch match;
            if (std::regex_search(input, match, regex) && !args.invert_match) {
                do {
                    print(match.prefix().str(), args.invert_match);
                    print(match.str(), !args.invert_match);
                    input = match.suffix().str();
                } while (std::regex_search(input, match, regex));
                print(input, args.invert_match);
                print("\n", false);
            }else if(!std::regex_search(input, match, regex) && args.invert_match){
                print(input, true);
                print("\n", false);
            }
        }
    }
    return 0;
}
