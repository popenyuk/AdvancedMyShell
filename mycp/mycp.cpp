#include <iostream>
#include <getopt.h>
#include <cstring>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/sendfile.h>
#include <unistd.h>

#include <errno.h>
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <dirent.h>
#include <vector>


#define DIR_TYPE 1
#define FILE_TYPE 2
typedef std::pair<int,int> File;
#define ERR_File File{-1, -1}
#define SKIP_File File{-2, -2}
#define ABORT_File File{-3, -3}

volatile bool force_flag = false;
bool recursive_flag = false;

static const char *optString = "Rfh";
static const struct option longOpts[] = {
        {"R",    no_argument, NULL, 'R'},
        {"f",    no_argument, NULL, 'f'},
        {"help", no_argument, NULL, 'h'},
        {NULL,   no_argument, NULL, 0}
};

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

File try_to_get_source(const char * filename) {
    struct stat stat_source;
    int source = open(filename, O_RDONLY, 0);
    if (source < 0) {
        std::cout << "[" << filename <<"] Error, during opening file: " << strerror(errno) << std::endl;
        return ERR_File;
    }
    fstat(source, &stat_source);
    if (S_ISDIR(stat_source.st_mode)) {
        return File{source, DIR_TYPE};
    }
    return File{source, FILE_TYPE};
}

File try_to_get_dest(const char * filename) {
    int dest = open(filename, O_WRONLY | O_CREAT | O_EXCL, 0644);
    if (dest < 0) {
        if (errno == EEXIST) {
            if (!force_flag) {
                std::cout << "File \"" << filename << "\" already exists, are you sure to replace it?" << std::endl <<"Y[es]/N[o]/A[ll]/C[ancel] Default: No>> ";
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
        std::cout << "[" << filename <<"] Error during opening file: " << strerror(errno) << std::endl;
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


std::string getFileName(const std::string& s) {
    char sep = '/';
    size_t i = s.rfind(sep, s.length());
    if (i != std::string::npos) {
        return(s.substr(i+1, s.length() - i));
    }
    return("");
}


void listdir(char *path, size_t size, std::vector<char *> & path_list ,std::vector<char *> & simplified_path_list, size_t prefix_length, const std::string &dir_to) {
    DIR *dir;
    struct dirent *entry;
    size_t len = strlen(path);

    if (!(dir = opendir(path))) {
        std::cerr <<  "path not found: " << path << " : " << strerror(errno) << std::endl;
        return;
    }

    while ((entry = readdir(dir)) != nullptr) {
        char *name = entry->d_name;
        if (entry->d_type == DT_DIR) {
            if (!strcmp(name, ".") || !strcmp(name, ".."))
                continue;
            if (len + strlen(name) + 2 > size) {
                std::cerr <<  "path too long: " << path << "/" << name << std::endl;
            } else {
                path[len] = '/';
                strcpy(path + len + 1, name);
                listdir(path, size, path_list, simplified_path_list, prefix_length, dir_to);
                path[len] = '\0';
            }
        } else {
            auto preprocessed_string = dir_to + std::string(path+prefix_length) + "/"+ name;
            char *preprocessed_c_str = new char[preprocessed_string.size() + 1];
            std::copy(&preprocessed_string[0], &preprocessed_string[0] + preprocessed_string.size() + 1, preprocessed_c_str);
            simplified_path_list.emplace_back(preprocessed_c_str);

            preprocessed_string = std::string(path) + "/"+ name;
            preprocessed_c_str = new char[preprocessed_string.size() + 1];
            std::copy(&preprocessed_string[0], &preprocessed_string[0] + preprocessed_string.size() + 1, preprocessed_c_str);
            path_list.emplace_back(preprocessed_c_str);


        }
    }
    closedir(dir);
}

void recursiveDirTraversal(char *path, std::vector<char *> & from, std::vector<char *> & to, const std::string &dir_to) {
    if (is_directory(path)) {
        char path_m[1024] = "../../sample/";
        listdir(path_m, sizeof path_m, from, to, strlen(path_m), dir_to);
    } else {
        auto preprocessed_string = dir_to + getFileName(path);
        char *preprocessed_c_str = new char[preprocessed_string.size() + 1];
        std::copy(&preprocessed_string[0], &preprocessed_string[0] + preprocessed_string.size() + 1, preprocessed_c_str);
        to.emplace_back(preprocessed_c_str);
        from.emplace_back(path);
    }
}

void free_vector_of_char_p(std::vector<char *> & vec) {
    for (auto entry : vec) {
        delete[] entry;
    }
}


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
        auto to = try_to_get_dest(argv[optind + 1]);
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
        auto dir_to = std::string{argv[argc-1]};
        if (dir_to[dir_to.size()-1]!='/') {
            dir_to += "/";
        }

        if (!is_directory(argv[argc-1])) {
            std::cerr << "Last argument must be a directory, while copying multiple files" << std::endl;
            return 1;
        }
        std::vector<char *> fromPathList;
        std::vector<char *> toPathList;
        if (recursive_flag) {
            for (int index = optind; index < argc-1; index ++) {
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

        std::vector<std::pair<int, char*>> from_fds;
        for (auto from_entry : fromPathList) {
            auto res = try_to_get_source(from_entry);
            if (res == ERR_File) {
                return 1;
            }
            from_fds.emplace_back(res.first, from_entry);
        }

        for (int i = 0; i < toPathList.size(); ++i) {
            auto to = try_to_get_dest(toPathList[i]);
            if (to == ERR_File) {
                return 1;
            }
            if (to == SKIP_File) {
                continue;
            } else if (to == ABORT_File) {
                return 0;
            } else {
                if (copy_raw(from_fds[i].first,to.first)<0) {
                    return 1;
                }
                close(from_fds[i].first);
                close(to.first);
            }
        }

        if (recursive_flag) {
            free_vector_of_char_p(fromPathList);
        }
        free_vector_of_char_p(toPathList);
    }

    return 0;
}