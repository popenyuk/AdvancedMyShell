#include <iostream>
#include <experimental/filesystem>
#include "argument_manager.h"

namespace filesystem = std::experimental::filesystem;

void rm_file(const std::string &file) {
    try {
        filesystem::remove(file);
    } catch (filesystem::filesystem_error &error) {
        std::cout << "Error occured with code: " << error.code() << std::endl;
        exit(-1);
    }
}

void remove_file(const std::string &file, bool &force) {
    if (!force) {
        std::string answer;
        std::cout << "Are you sure you want to delete this file " << file
                  << " ? Y[es]/N[o]/A[ll]/C[ancel]";
        std::cin >> answer;
        if (answer == "Y" || answer == "y") {
            rm_file(file);
        } else if (answer == "N" || answer == "n") {
            return;
        } else if (answer == "A" || answer == "a") {
            rm_file(file);
            force = true;
        } else if (answer == "C" || answer == "c") {
            exit(-1);
        } else {
            std::cout << "Unknown option " << answer << std::endl;
            exit(-1);
        }
    } else {
        rm_file(file);
    }
}

int main(int argc, char **argv) {
    if (argc == 1) {
        std::cout << "Wrong number of arguments." << std::endl;
        return -1;
    }
    arg_reader args;
    parse_arguments(argc, argv, args);
    if (args.help) {
        std::cout << "myrm [-h | --help] [-f] [-R] <file1> <file2> <file3>\n"
                     "\tOption -f - silently delete.\n"
                     "\tOption -R - delete directories as well (without this key must refuse to delete directories!)\n"
                     "\tIf the command is successful, the end code must be null, not null in case of error."
                  << std::endl;
    } else {
        for (auto &file: args.files) {
            if(!filesystem::exists(file)){
                std::cout << "There no " << file << std::endl;
                return -1;
            }
            if (filesystem::is_directory(file) && !args.recursive) {
                std::cout << file << " is a directory" << std::endl;
                return -1;
            }
            if (args.recursive && filesystem::is_directory(file)) {
                std::vector<std::string> back_files;
                for (auto &r_file : filesystem::recursive_directory_iterator(file)) {
                    back_files.insert(back_files.begin(), r_file.path().string());
                }
                for (auto &r_file:back_files) {
                    remove_file(r_file, args.force);
                }
                remove_file(file, args.force);
            } else {
                remove_file(file, args.force);
            }
        }
    }
    return 0;
}
