// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: http://www.viva64.com

#include "folder_creator.h"

#include <cstring>
#include <iostream>


#ifdef __linux__
#include <sys/stat.h>
#elif _WIN32

#include <windows.h>

#endif

int create_folder_p(const std::string &name) {
    size_t pos{0};
    std::string del;
#ifdef __linux__
    del = '/';
#elif _WIN32
    del = '\\';
#endif
    while (pos != name.size()) {
        if ((pos = name.find(del, pos + 1)) == std::string::npos) {
            pos = name.size();
        }
#ifdef __linux__
        if (mkdir(name.substr(0, pos).c_str(), 777) == -1 && errno != EEXIST) {
            std::cerr << "Error:  " << strerror(errno) << std::endl;
            return -1;
        }
#elif _WIN32
        int code;
        if (!CreateDirectory(name.substr(0, pos).c_str(), nullptr) && (code = GetLastError()) != ERROR_ALREADY_EXISTS) {
            std::cerr << "Error:  " << code << std::endl;
            return -1;
        }
#endif
    }
    return 0;
}

int create_folder(const std::string &name) {
#ifdef __linux__
    if (mkdir(name.c_str(), 777) == -1){
        std::cerr << "Error:  " << strerror(errno) << std::endl;
        return -1;
    }
#elif _WIN32
    int code;
    if (!CreateDirectory(name.c_str(), nullptr) && (code = GetLastError())) {
        std::cerr << "Error:  " << code << std::endl;
        return -1;
    }
#endif
    return 0;
}
