// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: http://www.viva64.com

#ifndef MYLS_FILE_H
#define MYLS_FILE_H

#include <ctime>
#include <chrono>
#include <string>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>

enum Mode {
    SIMPLE = 0, EXTENDED
};

class File {
    std::string name;
    struct stat fileStat{};
    unsigned char fileType{};

public:
    File() = default;

    explicit File(struct stat fileStat, std::string &filename);

    size_t getSize() const;

    size_t deviceNumber() const;

    size_t inodeNumber() const;

    bool isRegularFile() const;

    bool isExecutable() const;

    bool isDirectory() const;

    bool isCharacterDevice() const;

    bool isBlockDevice() const;

    bool isFifo() const;

    bool exist() const;

    bool isSymbolicLink() const;

    bool isSocket() const;

    std::string filePermissions() const;

    std::string lastModifiedTimeString() const;

    std::string getName() const;

    time_t lastModifiedTime() const;
};

inline std::ostream &operator<<(std::ostream &str, File const &f) {
    if ((f.isDirectory() || f.getName().find('/') != -1) && f.getName().find('/') != 0 && f.getName().find('.') != 0)
        return str << "/" + f.getName();
    else
        return str << f.getName();
}

#endif //MYLS_FILE_H
