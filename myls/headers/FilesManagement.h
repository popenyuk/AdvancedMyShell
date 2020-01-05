// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: http://www.viva64.com

#ifndef MYLS_FILESMANAGEMENT_H
#define MYLS_FILESMANAGEMENT_H

#include <vector>
#include <string>

#include "File.h"

class FilesManagement {
    std::vector<File> files;
    std::string filename;
    bool workingWithDirectory{};

    static File getFileFromName(std::string &name);

public:
    explicit FilesManagement(std::string &&filename, bool recursive);

    void analyzeFile(bool recursive);

    std::vector<File> get_files();

    void listDir(bool reverse, bool print_special_type);

    std::string getSpecialName(const File &file);

    void listDirWithDetailedInfo(bool reverse, bool print_special_type);

    void setFilePath(std::string filename_, bool recursive);

    void sortByString(const std::string &sorting_param);

    static struct stat getStat(std::string &&fileName);

    FilesManagement &sortBySize();

    FilesManagement &sortByTime();

    FilesManagement &sortByExtension();

    FilesManagement &dirsFirst();

    FilesManagement &specialFilesFirst();

    FilesManagement &sortByName();

    static std::vector<std::string> getFiles(std::string &&dirPath);
};

#endif //MYLS_FILESMANAGEMENT_H
