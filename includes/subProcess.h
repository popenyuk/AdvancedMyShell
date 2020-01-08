//
// Created by markiian on 07.01.20.
//

#ifndef MYSHELL_SUBPROCESS_H
#define MYSHELL_SUBPROCESS_H

#include <utility>
#include <string>
#include <vector>
#include <VariablesManager.h>
#include <sys/stat.h>

class subProcess {
private:
    const std::vector<std::string> my_optins{"merrno", "mpwd", "mcd", "mexit", "mecho", "mexport"};

    std::string commandName;
    std::vector<std::string> argsCopy;
    std::vector<char*> cstr_args;
    std::vector<char*> cstr_env;
    VariablesManager vm;

    int pid;
    int errcode;

public:
    int fd_in = -1;
    int fd_out = -1;
    int fd_err = -1;

    bool is_in_piped = false;
    std::pair<int,int> pipe_in;

    bool is_out_piped = false;
    std::pair<int,int> pipe_out;

    bool will_be_detached = false;

    bool is_command_builtin = false;
public:
    subProcess(std::string filename, std::vector<std::string> args, VariablesManager varMan);

    void start();
    int return_code();
    int wait();
    void pipe_to(subProcess & process);

};

inline bool checkIfFileexists (const std::string& name) {
    struct stat buffer;
    return (stat (name.c_str(), &buffer) == 0);
}

#endif //MYSHELL_SUBPROCESS_H
