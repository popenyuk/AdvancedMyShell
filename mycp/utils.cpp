//
// Created by markiian on 08.01.20.
//

#include "utils.h"

int copy_raw(int fd_from, int fd_to) {
    struct stat stat_from;
    fstat(fd_from, &stat_from);
    off_t offset = 0;
    int res = sendfile(fd_to, fd_from, &offset, stat_from.st_size);
    while (res > 0 && res < stat_from.st_size) {
        res = sendfile(fd_to, fd_from, &offset, stat_from.st_size);
    }

    if (res < 0) {
        std::cout << "Error during copying file: " << strerror(errno) << std::endl;
        return -1;
    }

    return 0;
}

File try_to_get_source(const char *filename) {
    struct stat stat_source;
    int source = open(filename, O_RDONLY, 0);
    if (source < 0) {
        std::cout << "[" << filename << "] Error, during opening file: " << strerror(errno) << std::endl;
        return ERR_File;
    }
    fstat(source, &stat_source);
    if (S_ISDIR(stat_source.st_mode)) {
        return File{source, DIR_TYPE};
    }
    return File{source, FILE_TYPE};
}

std::string getPathWOFileName(const std::string &s) {
    char sep = '/';
    size_t i = s.rfind(sep, s.length());
    if (i != std::string::npos) {
        return (s.substr(0, i + 1));
    }
    return ("");
}

File try_to_get_dest(const char *filename, bool &force_flag) {
    int dest = open(filename, O_WRONLY | O_CREAT | O_EXCL, 0644);
    if (dest < 0) {
        if (errno == ENOENT) {
            std::stringstream ss(getPathWOFileName(filename));
            std::string current_dir;
            std::string current_path = ".";
            while (std::getline(ss, current_dir, '/')) {
                current_path += "/" + current_dir;
                mkdir(current_path.c_str(), 0700);
            }
            dest = open(filename, O_WRONLY | O_CREAT | O_EXCL, 0644);
        } else if (errno == EEXIST) {
            if (!force_flag) {
                std::cout << "File \"" << filename << "\" already exists, are you sure to replace it?" << std::endl
                          << "Y[es]/N[o]/A[ll]/C[ancel] Default: No>> ";
                std::string res;
                std::cin >> res;
                if (res == "Y" || res == "y") {
                    dest = open(filename, O_WRONLY | O_TRUNC, 0644);
                } else if (res == "N" || res == "n") {
                    return SKIP_File;
                } else if (res == "A" || res == "a") {
                    dest = open(filename, O_WRONLY | O_TRUNC, 0644);
                    force_flag = true;
                } else if (res == "C" || res == "c") {
                    return ABORT_File;
                } else {
                    return SKIP_File;
                }
            } else {
                dest = open(filename, O_WRONLY | O_TRUNC, 0644);
            }

        }
    }

    if (dest < 0) {
        std::cout << "[" << filename << "] Error during creating file: " << strerror(errno) << std::endl;
        return ERR_File;
    }

    return File{dest, FILE_TYPE};

}

int is_directory(const char *path) {
    struct stat statbuf;
    if (stat(path, &statbuf) != 0)
        return 0;
    return S_ISDIR(statbuf.st_mode);
}


std::string getFileName(const std::string &s) {
    char sep = '/';
    size_t i = s.rfind(sep, s.length());
    if (i != std::string::npos) {
        return (s.substr(i + 1, s.length() - i));
    }
    return ("");
}


void listdir(char *path, size_t size, std::vector<char *> &path_list, std::vector<char *> &simplified_path_list,
             size_t prefix_length, const std::string &dir_to) {
    DIR *dir;
    struct dirent *entry;
    size_t len = strlen(path);

    if (!(dir = opendir(path))) {
        std::cerr << "path not found: " << path << " : " << strerror(errno) << std::endl;
        return;
    }

    while ((entry = readdir(dir)) != nullptr) {
        char *name = entry->d_name;
        if (entry->d_type == DT_DIR) {
            if (!strcmp(name, ".") || !strcmp(name, ".."))
                continue;
            if (len + strlen(name) + 2 > size) {
                std::cerr << "path too long: " << path << "/" << name << std::endl;
            } else {
                path[len] = '/';
                strcpy(path + len + 1, name);
                listdir(path, size, path_list, simplified_path_list, prefix_length, dir_to);
                path[len] = '\0';
            }
        } else {
            auto preprocessed_string = dir_to + std::string(path + prefix_length) + "/" + name;
            char *preprocessed_c_str = new char[preprocessed_string.size() + 1];
            std::copy(&preprocessed_string[0], &preprocessed_string[0] + preprocessed_string.size() + 1,
                      preprocessed_c_str);
            simplified_path_list.emplace_back(preprocessed_c_str);

            preprocessed_string = std::string(path) + "/" + name;
            preprocessed_c_str = new char[preprocessed_string.size() + 1];
            std::copy(&preprocessed_string[0], &preprocessed_string[0] + preprocessed_string.size() + 1,
                      preprocessed_c_str);
            path_list.emplace_back(preprocessed_c_str);


        }
    }
    closedir(dir);
}


void recursiveDirTraversal(char *path, std::vector<char *> &from, std::vector<char *> &to, const std::string &dir_to) {
    if (is_directory(path)) {
        char path_m[1024];
        strcpy(path_m, path);
        listdir(path_m, sizeof path_m, from, to, strlen(path_m), dir_to);
    } else {
//         Copying all entries, even if they are in argv, to make code easier (freeing memory after execution)
        auto preprocessed_string = dir_to + getFileName(path);
        char *preprocessed_c_str = new char[preprocessed_string.size() + 1];
        std::copy(&preprocessed_string[0], &preprocessed_string[0] + preprocessed_string.size() + 1,
                  preprocessed_c_str);
        to.emplace_back(preprocessed_c_str);

        preprocessed_string = std::string(path);
        preprocessed_c_str = new char[preprocessed_string.size() + 1];
        std::copy(&preprocessed_string[0], &preprocessed_string[0] + preprocessed_string.size() + 1,
                  preprocessed_c_str);
        from.emplace_back(preprocessed_c_str);
    }
}

void free_vector_of_char_p(std::vector<char *> &vec) {
    for (auto entry : vec) {
        delete[] entry;
    }
}