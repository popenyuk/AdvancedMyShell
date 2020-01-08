//
// Created by markiian on 07.01.20.
//

#include "subProcess.h"
#include <unistd.h>
#include <stdexcept>
#include <wait.h>
#include <algorithm>
#include <my_functions.h>
#include <iostream>



subProcess::subProcess(std::string command, std::vector<std::string> args, VariablesManager varMan) {
    vm = varMan;
//    Check for built in command
    if (std::find(my_optins.begin(), my_optins.end(), command) != my_optins.end()) {
        is_command_builtin = true;
        argsCopy = args;
        argsCopy.insert(argsCopy.begin(), 1, command);
        return ;
    }

// Check for path command
    bool findInPath = false;
    for (auto & elem : vm.path) {
        if (checkIfFileexists(elem + command)) {
            commandName = elem+command;
            findInPath = true;
            break;
        }
    }

//    Check for simple command

    if (!findInPath) {
        if (checkIfFileexists(command)) {
            commandName = command;
        } else {
            throw std::runtime_error("Command does not exist");
        }
    }


    char *prog_name = new char[commandName.size() + 1];
    std::copy(&commandName[0], &commandName[0] + commandName.size() + 1, prog_name);
    cstr_args.push_back(prog_name);

    for( auto & entry : args) {
        char *argument = new char[entry.size() + 1];
        std::copy(&entry[0], &entry[0] + entry.size() + 1, argument);
        cstr_args.push_back(argument);
    }

    for( auto & entry : vm.globalVariables) {
        auto row = entry.first + "=" + entry.second;
        char * env_arg = new char[row.size()+1];
        std::copy(&row[0], &row[0] + row.size() + 1, env_arg);
        cstr_env.push_back(env_arg);

    }


    }

void subProcess::start() {
    pid = fork();

    if (pid == -1) {
        throw std::runtime_error("Fork failed");
    } else if (pid == 0) { // child
        if (fd_in!=-1 || is_in_piped) {
            if (is_in_piped) {
                dup2(pipe_in.first, STDIN_FILENO);
                close(pipe_in.second);
            } else {
                dup2(fd_in, STDIN_FILENO);
            }
        }

        if (fd_out!=-1 || is_out_piped) {
            if (is_out_piped) {
                dup2(pipe_out.first, STDOUT_FILENO);
                close(pipe_out.second);
            } else {
                dup2(fd_out, STDOUT_FILENO);
            }
        }

        if (fd_err!=-1) {
            dup2(fd_err, STDERR_FILENO);
        }

        if (will_be_detached) {
            if (fd_in == -1) {
                close(STDIN_FILENO);
            }
            if (fd_out == -1) {
                close(STDOUT_FILENO);
            }
            if (fd_err == -1) {
                close(STDERR_FILENO);
            }
        }
        if (is_command_builtin) {
            run_my_options(argsCopy, vm);
        } else {
            execve(commandName.c_str(), cstr_args.data(), cstr_env.data());
        }
    }

    if (is_in_piped)
        close(pipe_in.first);
    if (is_out_piped)
        close(pipe_out.first);

    for (auto arg: cstr_args)
        delete[] arg;

    for (auto arg: cstr_env)
        delete[] arg;



}

int subProcess::return_code() {
    return errcode;
}

int subProcess::wait() {
    if (will_be_detached)
        return -999;
    int result = 0;
    waitpid(pid, &result, 0);
    return result;
}

void subProcess::pipe_to(subProcess & process) {
    int p_fd[2];
    pipe(p_fd);

    is_out_piped = true;
    pipe_out = std::pair<int,int>(p_fd[1], p_fd[0]);

    process.is_in_piped = true;
    process.pipe_in = std::pair<int,int>(p_fd[0], p_fd[1]);
}


