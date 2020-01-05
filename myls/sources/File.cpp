// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: http://www.viva64.com

#pragma clang diagnostic push
#pragma ide diagnostic ignored "hicpp-signed-bitwise"

#include "File.h"

File::File(struct stat file_stat, std::string &filename) {
    fileStat = file_stat;
    name = filename;
}

size_t File::deviceNumber() const {
    return fileStat.st_dev;
}

bool File::isRegularFile() const {
    return S_ISREG(fileStat.st_mode);
}

bool File::isDirectory() const {
    return S_ISDIR(fileStat.st_mode);
}

bool File::isCharacterDevice() const {
    return S_ISCHR(fileStat.st_mode);
}

bool File::isBlockDevice() const {
    return S_ISBLK(fileStat.st_mode);
}

bool File::isFifo() const {
    return S_ISFIFO(fileStat.st_mode);
}

bool File::isSymbolicLink() const {
    return S_ISLNK(fileStat.st_mode);
}

bool File::isSocket() const {
    return S_ISSOCK(fileStat.st_mode);
}

std::string File::filePermissions() const {
    return (
            std::string{(S_ISDIR(fileStat.st_mode) ? "d" : "-")} +
            std::string{(fileStat.st_mode & S_IRUSR ? "r" : "-")} +
            std::string{((fileStat.st_mode & S_IWUSR) ? "w" : "-")} +
            std::string{((fileStat.st_mode & S_IXUSR) ? "x" : "-")} +
            std::string{((fileStat.st_mode & S_IRGRP) ? "r" : "-")} +
            std::string{((fileStat.st_mode & S_IWGRP) ? "w" : "-")} +
            std::string{((fileStat.st_mode & S_IXGRP) ? "x" : "-")} +
            std::string{((fileStat.st_mode & S_IROTH) ? "r" : "-")} +
            std::string{((fileStat.st_mode & S_IWOTH) ? "w" : "-")} +
            std::string{((fileStat.st_mode & S_IXOTH) ? "x" : "-")}
    );
}

time_t File::lastModifiedTime() const {
    // TODO: get the correct value of last file modification time
    time_t time = fileStat.st_mtim.tv_sec;
    return time;
}

std::string File::lastModifiedTimeString() const {
    char buff[32];
    auto time = lastModifiedTime();
    strftime(buff, 32, "%Y-%m-%d %H:%M", localtime(&time));
    return std::string{buff};
}

size_t File::inodeNumber() const {
    return fileStat.st_ino;
}

std::string File::getName() const {
    return name;
}

size_t File::getSize() const {
    return fileStat.st_size;
}

bool File::isExecutable() const {
    return fileStat.st_mode & S_IXUSR;
}

bool File::exist() const {
    struct stat newFileStat;
    return stat(name.c_str(), &newFileStat) == 0;
}

#pragma clang diagnostic pop