#include "my_io.h"

#include <fcntl.h>
#include <unistd.h>
#include <fstream>
#include <iostream>

int change_d(std::vector<std::string> &args) {
    int fd = 0;
    bool is_opened{false};
    for (size_t index = 1; index < args.size(); ++index) {
        if (args.at(index - 1) == ">" || args.at(index - 1) == "2>" ||
            (index < (args.size() - 1) && args.at(index + 1) == "2>&1")) {
            if (is_opened) {
                close(fd);
            }
            fd = open(args.at(index).c_str(), O_TRUNC | O_RDWR | O_CREAT, 0777);
            if (fd == -1) {
                return -1;
            }
            if (args.at(index - 1) == ">" || (index < (args.size() - 1) && args.at(index) == "2>&1")) {
                if (close(STDOUT_FILENO) == -1) {
                    return -1;
                }
                dup2(fd, STDOUT_FILENO);
            }
            if (args.at(index - 1) == "2>" || (index < (args.size() - 1) && args.at(index) == "2>&1")) {
                if (close(STDERR_FILENO) == -1) {
                    return -1;
                }
                dup2(fd, STDERR_FILENO);
            }
            is_opened = true;
        }
    }
    close(fd);
    for (size_t index = args.size() - 1; index != 0; --index) {
        if (index < (args.size() - 2) && args.at(index + 1) == "2>&1") {
            args.erase(args.begin() + index + 1);
            args.erase(args.begin() + index);
        }
        if (args.at(index - 1) == ">" || args.at(index - 1) == "2>" || args.at(index) == ">" || args.at(index) == "2>") {
            args.erase(args.begin() + index);
        }
    }
    return 0;
}

void return_d(const std::vector<int>& fds){
    dup2(fds.at(0), STDIN_FILENO);
    dup2(fds.at(1), STDOUT_FILENO);
    dup2(fds.at(2), STDERR_FILENO);
}

int read_input_from_file(std::vector<std::string> &args){
    size_t index = 0;
    std::ifstream infile;
    while(index < args.size()){
        if(args.at(index) == "<" && index == args.size() - 1){
            return -1;
        }
        if(args.at(index) == "<"){
            infile.open(args.at(index + 1));
            args.erase(args.begin() + index);
            args.erase(args.begin() + index);
            std::string word;
            while(infile >> word){
                args.insert(args.begin() + index, word);
                ++index;
            }
        }
        ++index;
    }
    return 0;
}
