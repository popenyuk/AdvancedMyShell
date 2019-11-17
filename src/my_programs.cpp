// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: http://www.viva64.com

#include <wait.h>
#include <iostream>
#include <unistd.h>

#include "my_programs.h"

void run_my_programs(std::vector<std::string> &args, const VariablesManager &variablesManager, bool wait, bool redirect,
                     bool conveer) {
    pid_t pid{-1};
    int pipe_status;
    int pipefds_input[2], pipefds_output[2], pipefds_error[2];
    std::vector<std::string> new_args;
    args.emplace_back("|");
    for (const auto &arg : args) {
        if (arg == "|") {
            pipe_status = pipe(pipefds_input);
            if (pipe_status == -1) {
                perror("Error creating the pipe");
                exit(EXIT_FAILURE);
            }

            pipe_status = pipe(pipefds_output);
            if (pipe_status == -1) {
                perror("Error creating the pipe");
                exit(EXIT_FAILURE);
            }

            pipe_status = pipe(pipefds_error);
            if (pipe_status == -1) {
                perror("Error creating the pipe");
                exit(EXIT_FAILURE);
            }

            pid = fork();
            if (pid == -1) {
                std::cerr << "Failed to fork()" << std::endl;
                exit(EXIT_FAILURE);
            } else if (pid == 0) {
                if(arg != args.at(args.size() - 1)){
                    dup2 (0, pipefds_input[0]);
                    dup2 (1, pipefds_output[1]);
                    dup2 (2 ,pipefds_error[1]);
                }else{
                    dup2 (pipefds_input[0], 0);
                    dup2 (pipefds_output[1], 1);
                    dup2 (2 ,pipefds_error[1]);
                }

                break;
            } else {
                new_args.clear();
            }
        }else {
            new_args.push_back(arg);
        }
    }
    if (pid == -1) {
        std::cerr << "Failed to fork()" << std::endl;
        exit(EXIT_FAILURE);
    } else if (pid > 0) {
        // We are parent process
        int status;
        if (conveer) {
            close(pipefds_input[0]);
            close(pipefds_output[1]);
            close(pipefds_error[1]);
        }
        if (!wait) {
            waitpid(pid, &status, 0);
        }
        if (conveer) {
            close(pipefds_input[1]);
            close(pipefds_output[0]);
            close(pipefds_error[0]);
        }
    } else {
        // We are the child
        // Add dot to PATH
        std::string victim_name(new_args[0]);
        if (!redirect && wait) {
            close(STDIN_FILENO);
            close(STDOUT_FILENO);
            close(STDERR_FILENO);
        }
        //! Вважаємо, що кількість аргументів на момент компіляції не відома!
        //! Така ситуація буде виникати у нас часто, насправді.
        //! Інакше немає потреби так складно робити.
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
        arg_for_c.reserve(new_args.size());
        for (const auto &s: new_args)
            arg_for_c.push_back(s.c_str());
        arg_for_c.push_back(nullptr);
        //! const_cast використано, так як не бачу іншого способу
        //! з const char** отримати char* const*, який хоче ф-ція
        execvp(victim_name.c_str(), const_cast<char *const *>(arg_for_c.data()));
        exit(EXIT_FAILURE);   // exec never returns
    }
}
