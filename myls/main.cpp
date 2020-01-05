// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: http://www.viva64.com
#include <iostream>
#include <sys/stat.h>
#include <functional>
#include <wildcard_parser.h>

#include "File.h"
#include "FilesManagement.h"
#include "arguments_reader.h"


int main(int argc, char *argv[]) {
    arg_reader my_reader;
    read_arguments(argc, argv, std::ref(my_reader));
    if (my_reader.help) {
        std::cout << "Usage: ls [OPTION]... [FILE]...\n"
                     "List information about the FILEs (the current directory by default).\n"
                     "Oprions:\n"
                     "\t-r - reverse order while sorting\n"
                     "\t-l - list one file per line.  Avoid '\\n' with -q or -b\n"
                     "\t--sort=<Xx>\t- specifies the sort criteria\n"
                     "\t\tU - not sort [Unsorted],\n"
                     "\t\tS - by size [Size],\n"
                     "\t\tt - by the last modification [time],\n"
                     "\t\tX - by extension,\n"
                     "\t\tN - by default (default) [Name].\n"
                     "\t\tEither of these can be one or both of the following two modifiers:\n"
                     "\t\tD - first display the [Directories_first] directories,\n"
                     "\t\ts - Specify special files (see below) separately [special_file].\n"
                     "\t-F -  append indicator (one of */=@?) to entries\n"
                     "\t-R - access all directories recursively, listing their contents.\n"
                     "\t -- (two hyphens) - All after them are considered to be file or directory names. "
                     "To it, starting with '-' is considered an option." << std::endl;
    } else {
        FilesManagement fm(".", my_reader.recursive);
        if (my_reader.pathes.empty()) {
            if (!my_reader.sort.empty()) {
                fm.sortByString(my_reader.sort);
            }
            if (my_reader.full_information) {
                fm.listDirWithDetailedInfo(my_reader.r, my_reader.f);
            } else {
                fm.listDir(my_reader.r, my_reader.f);
            }
        } else {
            std::vector<File> files{};
            for(auto& f: my_reader.pathes){
                if (is_wildcard(f)){
                    auto fs = get_matches(fm.get_files(), f);
                    files.insert(std::end(files), std::begin(fs), std::end(fs));
                }
            }
            for(auto& f: files){
                std::cout << f << std::endl;
            }
            return 0;
            for (size_t index = 0; index < my_reader.pathes.size(); ++index) {
                if (my_reader.first_dir) {
                    if (index == 0 && my_reader.pathes.size() != 1) {
                        continue;
                    } else if (my_reader.pathes.size() == 1) {
                        fm.setFilePath(my_reader.pathes.at(0), my_reader.recursive);
                    } else {
                        fm.setFilePath(my_reader.pathes.at(0) + my_reader.pathes.at(index), my_reader.recursive);
                    }
                } else {
                    fm.setFilePath(my_reader.pathes.at(index), my_reader.recursive);
                }
                if (!my_reader.sort.empty()) {
                    fm.sortByString(my_reader.sort);
                }
                if (my_reader.full_information) {
                    fm.listDirWithDetailedInfo(my_reader.r, my_reader.f);
                } else {
                    fm.listDir(my_reader.r, my_reader.f);
                }
            }
        }
    }
    return 0;
}