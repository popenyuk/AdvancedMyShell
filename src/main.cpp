// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: http://www.viva64.com

#include <regex>
#include <iostream>
#include <unistd.h>

#include "Input.h"
#include "my_io.h"
#include "merrno.h"
#include "my_programs.h"
#include "my_functions.h"
#include "VariablesManager.h"

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wmissing-noreturn"

merrno my_errno;

int main(int argc, char **argv) {
    std::cout << "Welcome! You can exit by pressing Ctrl+C at any time...\n" << std::endl;
    auto variablesManager = VariablesManager();

    std::stringstream mstringstream;
    mstringstream << "/bin/:/bin/local/:" << get_current_directory();
    variablesManager.setGlobalVariable("PATH", mstringstream.str());

    std::vector<std::string> my_optins{"merrno", "mpwd", "mcd", "mexit", "mecho", "mexport"};
    std::vector<std::string> my_programs{"mycat"};

    std::regex varDeclaration(R"([a-zA-Z]+=.+)");
    std::regex extCommand(R"(^(.\/|..\/|\/).+)");
    std::vector<int> fds{dup(STDIN_FILENO), dup(STDOUT_FILENO), dup(STDERR_FILENO)};
    while (true) {
        try {
            Input inp{variablesManager};

            auto res = inp.preprocessCommand();

            if (res.empty()) continue;

            if (res.size() == 1 && std::regex_match(res[0], varDeclaration)) {
                auto delimiterPos = res[0].find('=');
                variablesManager.setLocalVariable(res[0].substr(0, delimiterPos), res[0].substr((delimiterPos + 1)));
            } else {
                bool conveer{false};
                bool infromfile{false};
                bool wait{false};
                bool redirect{false};
                for (auto &re:res) {
                    if (re == "|") {
                        conveer = true;
                    }
                    if (re == "<") {
                        infromfile = true;
                    }
                    if (re == "&") {
                        wait = true;
                    }
                    if (re == "&" && re != res.at(res.size() - 1)) {
                        std::cerr << "Syntax error! & Must be last!" << std::endl;
                        continue;
                    }
                    if (re == ">" || re == "2>" || re == "2>&1") {
                        redirect = true;
                    }
                }
                if (redirect && change_d(res) != 0) {
                    return_d(fds);
                    std::cerr << "Can`t redirect output" << std::endl;
                    continue;
                }
                if (infromfile) {
                    read_input_from_file(res);
                }
                if (std::find(my_optins.begin(), my_optins.end(), res[0]) != my_optins.end()) {
                    run_my_options(res, variablesManager);
                } else {
                    run_my_programs(res, variablesManager, wait, redirect, conveer);
                }
                return_d(fds);
            }

            if (my_errno.get_code() != 0) {
                my_errno.get_description();
            }
        } catch (std::runtime_error &error) {
            std::cerr << error.what() << std::endl;
        } catch (...) {
            std::cerr << "Unknown error." << std::endl;
        }
    }
    return 0;
}

#pragma clang diagnostic pop