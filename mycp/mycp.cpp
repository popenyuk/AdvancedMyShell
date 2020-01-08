#include <iostream>
#include <getopt.h>
#include "utils.h"

bool recursive_flag = false;
bool force_flag = false;

static const char *optString = "Rfh";
static const struct option longOpts[] = {
        {"R",    no_argument, NULL, 'R'},
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
                std::cout << "My analog of cp\nmycp [-h|--help] [-R] [-f] <file1> <file2> ... <fileN>" << std::endl;
                return 0;
            case 'f':
                force_flag = true;
                break;
            case 'R':
                recursive_flag = true;
                break;
            default:
                std::cout << "Error occured during args parsing, consider running \"./mycp --help\"" << std::endl;
                return 1;
        }

    if (argc - optind < 2) {
        std::cout << "Too few arguments, consider running \"./mycp --help\"" << std::endl;
        return 1;
    }
    // endregion


    if (argc - optind == 2) {
        auto from = try_to_get_source(argv[optind]);
        auto to = try_to_get_dest(argv[optind + 1], force_flag);
        if (from.second == DIR_TYPE) {
            std::cerr << "mycd cannot copy directories" << std::endl;
            return 1;
        }
        if (from == ERR_File || to == ERR_File) {
            return 1;
        }

        if (to == SKIP_File || to == ABORT_File) {
            return 0;
        }
        return copy_raw(from.first, to.first);
    } else {
        auto dir_to = std::string{argv[argc - 1]};
        if (dir_to[dir_to.size() - 1] != '/') {
            dir_to += "/";
        }

        if (!is_directory(argv[argc - 1])) {
            std::cerr << "Last argument must be a directory, while copying multiple files" << std::endl;
            return 1;
        }
        std::vector<char *> fromPathList;
        std::vector<char *> toPathList;
        if (recursive_flag) {
            for (int index = optind; index < argc - 1; index++) {
                recursiveDirTraversal(argv[index], fromPathList, toPathList, dir_to);
            }
        } else {
            for (int index = optind; index < argc - 1; index++) {
                if (is_directory(argv[index])) {
                    std::cerr << "Error, cannot copy directories without -R flag" << std::endl;
                    free_vector_of_char_p(toPathList);
                    return 1;
                }
                recursiveDirTraversal(argv[index], fromPathList, toPathList, dir_to);
            }
        }

        std::vector<std::pair<int, char *>> from_fds;
        for (auto from_entry : fromPathList) {
            auto res = try_to_get_source(from_entry);
            if (res == ERR_File) {
                return 1;
            }
            from_fds.emplace_back(res.first, from_entry);
        }

        for (int i = 0; i < toPathList.size(); ++i) {
            auto to = try_to_get_dest(toPathList[i], force_flag);
            if (to == ERR_File) {
                return 1;
            }
            if (to == SKIP_File) {
                continue;
            } else if (to == ABORT_File) {
                return 0;
            } else {
                if (copy_raw(from_fds[i].first, to.first) < 0) {
                    return 1;
                }
                close(from_fds[i].first);
                close(to.first);
            }
        }

        free_vector_of_char_p(fromPathList);
        free_vector_of_char_p(toPathList);
    }

    return 0;
}