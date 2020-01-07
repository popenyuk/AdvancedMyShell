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

subProcess::subProcess(std::string command, std::vector<std::string> & args, VariablesManager varMan) {
    vm = varMan;
//    Check for built in command
    if (std::find(my_optins.begin(), my_optins.end(), command) != my_optins.end()) {
        is_command_builtin = true;
        argsCopy = args;
        argsCopy.insert(argsCopy.begin(), 1, command);
        return ;
    }

// Check for path command
// TODO Check for path command
//    Check for simple command
    if (!checkIfFileexists(command)) {
        throw std::runtime_error("Command does not exist");
    }
    commandName = command;
    for( auto & entry : args) {
        char *argument = new char[entry.size() + 1];
        std::copy(&entry[0], &entry[0] + entry.size() + 1, argument);
        cstr_args.push_back(argument);
    }

}

void subProcess::start() {
    pid = fork();

    if (pid == -1) {
        throw std::runtime_error("Fork failed");
    } else if (pid == 0) { // child
        if (fd_in!=-1 || is_in_piped) {
            if (is_in_piped) {
                dup2(pipe_in.first, 0);
                close(pipe_in.second);
            } else {
                dup2(fd_in, 0);
            }
        }

        if (fd_out!=-1 || is_out_piped) {
            if (is_out_piped) {
                dup2(pipe_out.first, 1);
                close(pipe_out.second);
            } else {
                dup2(fd_out, 1);
            }
        }

        if (fd_err!=-1) {
            dup2(fd_err, 2);
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
//        execve(commandName.c_str(), "tokens.data()", "env.getEnvironmentVariables()");
        if (is_command_builtin) {
            run_my_options(argsCopy, vm);
        } else {
            execve(commandName.c_str(), cstr_args.data(), NULL);
        }
    }

    if (is_in_piped)
        close(pipe_in.first);

    if (is_out_piped)
        close(pipe_out.first);

    for (auto & entry : cstr_args) {
        delete [] entry;
    }
}

int subProcess::return_code() {
    return errcode;
}

void subProcess::wait() {
    if (will_be_detached)
        return;
    waitpid(pid, &errcode, 0);
}

void subProcess::pipe_to(subProcess & process) {
    int p_fd[2];
    pipe(p_fd);

    is_out_piped = true;
    pipe_out = std::pair<int,int>(p_fd[1], p_fd[0]);

    process.is_in_piped = true;
    process.pipe_in = std::pair<int,int>(p_fd[0], p_fd[1]);
};

