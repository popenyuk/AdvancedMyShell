// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: http://www.viva64.com

#include <wait.h>
#include <iostream>
#include <unistd.h>

#include "my_programs.h"

void origin_fork(std::vector<std::string> &args, const VariablesManager &variablesManager, bool wait, bool redirect,
                 pid_t pid = -1) {
    if (pid == -1) {
        pid = fork();
    }

    if (pid == -1) {
        std::cerr << "Failed to fork()" << std::endl;
        exit(EXIT_FAILURE);
    } else if (pid > 0) {
        // We are parent process
        int status;
        if (!wait) {
            waitpid(pid, &status, 0);
        }
    } else {
        // We are the child
        // Add dot to PATH
        std::string victim_name(args[0]);
        //! Вважаємо, що кількість аргументів на момент компіляції не відома!
        //! Така ситуація буде виникати у нас часто, насправді.
        //! Інакше немає потреби так складно робити.
        if (wait && !redirect) {
            close(STDIN_FILENO);
            close(STDOUT_FILENO);
            close(STDERR_FILENO);
        }

        auto path_ptr = getenv("PATH");
        std::string path_var;
        if (path_ptr != nullptr)
            path_var = path_ptr;
        path_var += ":";
        path_var.insert(path_var.size(), variablesManager.getAllGlobalVariable());
        path_var += ":.";
        setenv("PATH", path_var.c_str(), 1);
        //! Environment is ready
        //! Prepare args array in form suitable for execvp
        std::vector<const char *> arg_for_c;
        arg_for_c.reserve(args.size());
        for (const auto &s: args)
            arg_for_c.push_back(s.c_str());
        arg_for_c.push_back(nullptr);
        //! const_cast використано, так як не бачу іншого способу
        //! з const char** отримати char* const*, який хоче ф-ція
        execvp(victim_name.c_str(), const_cast<char *const *>(arg_for_c.data()));
        exit(EXIT_FAILURE);   // exec never returns
    }
}

void run_my_programs(std::vector<std::string> &args, const VariablesManager &variablesManager, bool wait, bool redirect,
                     bool conveer) {
    if (!conveer) {
        origin_fork(args, variablesManager, wait, redirect);
    } else {
        args.emplace_back("|");
        std::vector<std::vector<std::string>> pipeline;
        std::vector<std::string> new_pipeline;
        for (auto &arg:args) {
            if (arg == "|") {
                pipeline.push_back(new_pipeline);
                new_pipeline.clear();
            } else {
                new_pipeline.push_back(arg);
            }
        }
        int my_pipe[2];
        for (size_t index = 0; index < pipeline.size(); ++index) {
            if (index == 0) {
                if (pipe(my_pipe) == -1) {
                    std::cerr << "pipe error" << std::endl;
                    exit(EXIT_FAILURE);
                }
                pid_t pid = fork();
                if (pid == -1) {
                    std::cerr << "Failed to fork()" << std::endl;
                    exit(EXIT_FAILURE);
                } else if (pid == 0) {
                    dup2(my_pipe[1], STDOUT_FILENO);
                    close(my_pipe[0]);
                } else {
                    close(my_pipe[0]);
                }
                origin_fork(pipeline.at(index), variablesManager, false, true, pid);
            } else if (index != pipeline.size() - 1) {
                pid_t pid = fork();
                if (pid == -1) {
                    std::cerr << "Failed to fork()" << std::endl;
                    exit(EXIT_FAILURE);
                } else if (pid == 0) {
                    close(my_pipe[1]);
                    dup2(my_pipe[0], STDIN_FILENO);
                }
                origin_fork(pipeline.at(index), variablesManager, false, true, pid);
                if (pipe(my_pipe) == -1) {
                    std::cout << "pipe error" << std::endl;
                    exit(EXIT_FAILURE);
                }
                dup2(my_pipe[1], STDOUT_FILENO);
            } else {
                pid_t pid = fork();
                if (pid == -1) {
                    std::cerr << "Failed to fork()" << std::endl;
                    exit(EXIT_FAILURE);
                } else if (pid == 0) {
                    dup2(my_pipe[0], STDOUT_FILENO);
                }
                origin_fork(pipeline.at(index), variablesManager, wait, true, pid);
            }
        }
    }
}
