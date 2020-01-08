//
// Created by markiian on 08.01.20.
//

#ifndef MYFUNCPACK_UTILS_H
#define MYFUNCPACK_UTILS_H

#include <cstring>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/sendfile.h>
#include <cerrno>
#include <sys/types.h>
#include <sstream>
#include <vector>
#include <iostream>
#include <dirent.h>
#include <unistd.h>


#define DIR_TYPE 1
#define FILE_TYPE 2
typedef std::pair<int, int> File;
#define ERR_File File{-1, -1}
#define SKIP_File File{-2, -2}
#define ABORT_File File{-3, -3}

int copy_raw(int fd_from, int fd_to);

File try_to_get_source(const char *filename);

std::string getPathWOFileName(const std::string &s);

File try_to_get_dest(const char *filename, bool &force_flag);

int is_directory(const char *path);

std::string getFileName(const std::string &s);

void listdir(char *path, size_t size, std::vector<char *> &path_list, std::vector<char *> &simplified_path_list,
             size_t prefix_length, const std::string &dir_to);

void recursiveDirTraversal(char *path, std::vector<char *> &from, std::vector<char *> &to, const std::string &dir_to);

void free_vector_of_char_p(std::vector<char *> &vec);

#endif //MYFUNCPACK_UTILS_H
