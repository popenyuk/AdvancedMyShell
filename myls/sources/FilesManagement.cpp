// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: http://www.viva64.com

#include <cstring>
#include <iomanip>
#include <iostream>
#include <dirent.h>
#include <algorithm>
#include <utility>

#include "FilesManagement.h"

FilesManagement::FilesManagement(std::string &&filename_, bool recursive) {
    filename = filename_;
    analyzeFile(recursive);
}

void FilesManagement::setFilePath(std::string filename_, bool recursive) {
    files.clear();
    filename = std::move(filename_);
    analyzeFile(recursive);
}

File FilesManagement::getFileFromName(std::string &name) {
    auto stat_ = getStat(std::string{name});
    return File(stat_, name);
}

void FilesManagement::analyzeFile(bool recursive) {
    std::string cur_dir{filename};
    auto f = getFileFromName(cur_dir);
    if (f.isDirectory()) {
        workingWithDirectory = true;
        for (auto i: getFiles(f.getName())) {
            files.push_back(getFileFromName(i));
            if (recursive && getFileFromName(i).isDirectory()) {
                filename = i + '/';
                this->analyzeFile(recursive);
            }
        }
    } else {
        workingWithDirectory = false;
        files.push_back(f);
    }
}

std::vector<std::string> FilesManagement::getFiles(std::string &&dirPath) {
    DIR *dirp;
    struct dirent *dp;
    bool isCurrent;
    std::vector<std::string> relative_pathes{};

    isCurrent = strcmp(dirPath.c_str(), ".") == 0;
    dirp = opendir(dirPath.c_str());
    if (dirp == nullptr) {
        int errno_copy = errno;
        printf("Failed opening dir with errno = %i\n", errno_copy);
        printf("\t%s\n", strerror(errno_copy));
        return std::vector<std::string>{};
    }

    for (;;) {
        errno = 0;
        dp = readdir(dirp);
        std::string path;
        if (dp == nullptr)
            break;
        if (strcmp(dp->d_name, ".") == 0 || strcmp(dp->d_name, "..") == 0)
            continue;
        if (!isCurrent)
            path = dirPath;
        path += dp->d_name;
        relative_pathes.push_back(path);
    }
    if (errno != 0) {
        int errno_copy = errno;
        printf("Error while traversing directory with errno = %i\n", errno_copy);
        printf("\t%s\n", strerror(errno_copy));
        return std::vector<std::string>{};
    }
    if (closedir(dirp) == -1) {
        int errno_copy = errno;
        printf("Error closing directory with errno = %i\n", errno_copy);
        printf("\t%s\n", strerror(errno_copy));
        return std::vector<std::string>{};
    }
    return relative_pathes;
}

struct stat FilesManagement::getStat(std::string &&fileName) {
    struct stat fileStats{};
    stat(fileName.c_str(), &fileStats);
    return fileStats;
}

void FilesManagement::listDirWithDetailedInfo(bool reverse, bool print_special_type) {
    if (reverse)
        std::reverse(files.begin(), files.end());
    std::cout << std::setfill(' ') << std::left;
    for (const auto &i: files) {
        std::string str{getSpecialName(i)};
        if (i.exist()) {
            std::cout << std::setw(17) << i.filePermissions() <<
                      std::setw(30) << i << std::setw(30) << str << std::setw(30) <<
                      i.lastModifiedTimeString() << i.getSize() << std::endl;
        } else {
            std::cerr << "Unknown file " << i.getName() << std::endl;
        }
    }
}

std::string FilesManagement::getSpecialName(const File &file) {
    if (file.isSymbolicLink()) {
        return "@";
    } else if (file.isSocket()) {
        return "=";
    } else if (file.isFifo()) {
        return "|";
    } else if (file.isRegularFile()) {
        return "*";
    } else if (file.isDirectory()) {
        return "/";
    } else {
        return "?";
    }
}

void FilesManagement::listDir(bool reverse, bool print_special_type) {
    if (reverse)
        std::reverse(files.begin(), files.end());
    for (const auto &i: files) {
        std::string str{getSpecialName(i)};
        if (i.exist()) {
            std::cout << i << str << std::endl;
        } else {
            std::cerr << "Unknown file " << i.getName() << std::endl;
        }
    }
}

FilesManagement &FilesManagement::dirsFirst() {
    for (auto it = files.begin(); it != files.end(); ++it) {
        if (it->isDirectory()) {
            auto x = std::move(*it);
            files.erase(it);
            files.insert(files.begin(), std::move(x));
        }
    }
    return *this;
}

FilesManagement &FilesManagement::specialFilesFirst() {
    for (auto it = files.begin(); it != files.end(); ++it) {
        if (!getSpecialName(*it).empty() && !it->isDirectory()) {
            auto x = std::move(*it);
            files.erase(it);
            files.insert(files.begin(), std::move(x));
        }
    }
    return *this;
}

FilesManagement &FilesManagement::sortBySize() {
    std::sort(files.begin(), files.end(), [](const File &a, const File &b) {
        return a.getSize() > b.getSize();
    });
    return *this;
}

FilesManagement &FilesManagement::sortByTime() {
    std::sort(files.begin(), files.end(), [](const File &a, const File &b) {
        return a.lastModifiedTime() < b.lastModifiedTime();
    });
    return *this;
}

FilesManagement &FilesManagement::sortByName() {
    std::sort(files.begin(), files.end(), [](const File &a, const File &b) {
        return a.getName() < b.getName();
    });
    return *this;
}

FilesManagement &FilesManagement::sortByExtension() {
    std::sort(files.begin(), files.end(), [](const File &a, const File &b) {
        return a.getName().substr(a.getName().rfind('.')) < b.getName().substr(b.getName().rfind('.'));
    });
    return *this;
}

void FilesManagement::sortByString(const std::string &sorting_param) {
    if (sorting_param.find('U') != -1)
        return;
    for (auto &p:sorting_param) {
        switch (p) {
            case 'S':
                this->sortBySize();
                break;
            case 't':
                this->sortByTime();
                break;
            case 'X':
                this->sortByExtension();
                break;
            case 'N':
                this->sortByName();
                break;
            case 'D':
                this->dirsFirst();
            case 's':
                this->specialFilesFirst();
                break;
            default:
                std::cerr << "Unknown parameter " << p << "." << std::endl;
                return;
        }
    }
}

std::vector<File> FilesManagement::get_files() {
    return files;
}
