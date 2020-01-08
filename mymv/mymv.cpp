#include <iostream>
#include <getopt.h>
#include "utils.h"

bool force_flag = false;

static const char *optString = "fh";
static const struct option longOpts[] = {
        {"f",    no_argument, NULL, 'f'},
        {"help", no_argument, NULL, 'h'},
        {NULL,   no_argument, NULL, 0}
};

int main(int argc, char *argv[]) {
    //    region Options parsing
    int longIndex = 0;
    int opt;

    while ((opt = getopt_long(argc, argv, optString, longOpts, &longIndex)) != -1)
        switch (opt) {
            case 'h':
                std::cout << "My analog of mv\nmymv [-h|--help] [-f] <file1> <file2> ... <fileN>" << std::endl;
                return 0;
            case 'f':
                force_flag = true;
                break;
            default:
                std::cout << "Error occured during args parsing, consider running \"./mycp --help\"" << std::endl;
                return 1;
        }

    if (argc - optind < 2) {
        std::cout << "Too few arguments, consider running \"./mymv --help\"" << std::endl;
        return 1;
    }
    // endregion


    if (argc - optind == 2) {
        std::string to_edited(argv[optind + 1]);
        if (to_edited[0] == '.' && to_edited[1] == '.') {
            to_edited.erase(0, 2);
        }
        return move_pipeline(argv[optind], to_edited.c_str(), force_flag);
    } else {
        auto dir_to = std::string{argv[argc - 1]};
        if (dir_to[dir_to.size() - 1] != '/') {
            dir_to += "/";
        }

        if (!is_directory(argv[argc - 1])) {
            std::cerr << "Last argument must be a directory, while moving multiple files" << std::endl;
            return 1;
        }

//        In task not specified, so processing every entry one by one
        for (int index = optind; index < argc - 1; index++) {
            auto filename_to_move = std::string{argv[index]};
            if (filename_to_move[filename_to_move.size() - 1] == '/') {
                filename_to_move.pop_back();
            }

            std::string to_edited(filename_to_move);
            if (to_edited[0] == '.' && to_edited[1] == '.') {
                to_edited.erase(0, 2);
            }

            if (move_pipeline(filename_to_move.c_str(), (dir_to + to_edited).c_str(), force_flag) != 0) {
                return 1;
            }
        }
    }

    return 0;
}