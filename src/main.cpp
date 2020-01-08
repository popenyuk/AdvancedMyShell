// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: http://www.viva64.com

#include <regex>
#include <iostream>
#include <unistd.h>
#include <subProcess.h>

#include "Input.h"
#include "my_io.h"
#include "merrno.h"
//#include "my_programs.h"
#include "my_functions.h"
#include "my_file_reader.h"
#include "VariablesManager.h"
#include <fcntl.h>
//#include <MyProcess.h>

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wmissing-noreturn"

merrno my_errno;

int main(int argc, char **argv) {
    if (argc == 1)
        std::cout << "Welcome! You can exit by pressing Ctrl+C at any time...\n" << std::endl;
    auto variablesManager = VariablesManager();

    std::stringstream mstringstream;
    mstringstream << "/bin/:/bin/local/:/usr/bin/:" << get_current_directory();
    variablesManager.setGlobalVariable("PATH", mstringstream.str());

    std::regex varDeclaration(R"([a-zA-Z]+=.+)");
    std::regex extCommand(R"(^(.\/|..\/|\/).+)");

//    std::vector<int> fds{dup(STDIN_FILENO), dup(STDOUT_FILENO), dup(STDERR_FILENO)};
    std::vector<std::vector<std::string>> commands;
    size_t index = 0;
    bool prefix{false};
    bool end_of_program{false};
    bool is_file{false};
    if (argc != 1) {
        commands = read_from_file(argv[1]);
        if (commands.at(0).at(0) == "#") {
            index = 1;
            prefix = true;
        }
        end_of_program = true;
    }
    while (true) {
        try {
            Input inp{variablesManager};
            std::vector<std::string> res;
            if (argc == 1 && !is_file) {
                res = inp.preprocessCommand();
                if (res.at(0) == "." && res.size() == 2) {
                    commands = read_from_file(res[1]);
                    if (commands.at(0).at(0) == "#" && commands.at(0).at(0) == "!") {
                        prefix = true;
                    }
                    is_file = true;
                    continue;
                } else if (res.size() == 1) {
                    try {
                        commands = read_from_file(res[0]);
                        if (commands.at(0).at(0) == "#" && commands.at(0).at(0) == "!") {
                            is_file = true;
                            prefix = true;
                            continue;
                        }
                    } catch (...) {}
                }
            } else {
                if (index >= commands.size() && end_of_program) {
                    break;
                }
                if (index >= commands.size() && !end_of_program) {
                    is_file = false;
                    index = 0;
                    continue;
                }
                if (prefix) {
                    commands.at(index).insert(commands.at(index).begin(), commands.at(0).at(0).substr(2));
                }
                res = commands.at(index);
                ++index;
            }

            if (res.empty()) continue;

            if (res.size() == 1 && std::regex_match(res[0], varDeclaration)) {
//                Var creating
                auto delimiterPos = res[0].find('=');
                variablesManager.setLocalVariable(res[0].substr(0, delimiterPos), res[0].substr((delimiterPos + 1)));
            } else {
//                Command running
                std::vector<subProcess> pipeline;

                std::string command;
                std::vector<std::string> args;

                std::string in_redirected{};
                std::string out_redirected{};
                std::string err_redirected{};
                bool flag_will_be_detached = false;
                bool flag_in_err_redirected = false;

                for (int i = res.size()-1; i >= 0; i--) {
                    if (res[i] == "|" || i == 0) {

                        if (i==0) {
//                            args.push_back(res[])
                            std::reverse(args.begin(), args.end());
                            pipeline.emplace_back(res[i], args, variablesManager);
                        } else {
                            args.pop_back();
                            std::reverse(args.begin(), args.end());
                            pipeline.emplace_back(res[i+1], args, variablesManager);

                        }
                        args.clear();

                        if (!in_redirected.empty()) {
                            int fd = open(in_redirected.c_str(), O_RDWR | O_CREAT, S_IRWXU);
                            if (fd < 0) {
                                throw std::runtime_error("Cannot open file");
                            }
                            in_redirected.clear();
                            pipeline[pipeline.size()-1].fd_in = fd;
                        }

                        if (!out_redirected.empty()) {
                            int fd = open(out_redirected.c_str(), O_RDWR | O_CREAT, S_IRWXU);
                            if (fd < 0) {
                                throw std::runtime_error("Cannot open file");
                            }
                            out_redirected.clear();
                            pipeline[pipeline.size()-1].fd_out = fd;
                            if (flag_in_err_redirected) {
                                pipeline[pipeline.size()-1].fd_err = fd;
                                flag_in_err_redirected = false;
                            }
                        }

                        if (!err_redirected.empty()) {
                            int fd = open(err_redirected.c_str(), O_RDWR | O_CREAT, S_IRWXU);
                            if (fd < 0) {
                                throw std::runtime_error("Cannot open file");
                            }
                            err_redirected.clear();
                            pipeline[pipeline.size()-1].fd_in = fd;
                        }

                        pipeline[pipeline.size()-1].will_be_detached = flag_will_be_detached;
                        flag_will_be_detached = false;

                    }  else if (res[i] == "<") {
                        in_redirected = args.back();
                        args.pop_back();

                    } else if (res[i] == "&") {
                        if (i!= res.size()-1) {
                            throw std::runtime_error("Syntax error");
                        } else {
                            flag_will_be_detached = true;
                        }
                    } else if (res[i]==">") {
                        out_redirected = args.back();
                        args.pop_back();

                    } else if (res[i]=="2>") {
                        err_redirected = args.back();
                        args.pop_back();

                    } else if (res[i]=="2>&1") {
                        flag_in_err_redirected = true;
                    } else {
                        args.push_back(res[i]);
                    }
                }
                int exit_code;
                if (pipeline.size()==1) {
                    pipeline[0].start();
                    exit_code = pipeline[0].wait();
                } else {
                    for (int i = pipeline.size()-1; i >=1; --i) {
                        pipeline[i].pipe_to(pipeline[i-1]);
                    }

                    for (int i = pipeline.size()-1; i >=0; --i) {
                        pipeline[i].start();
                    }

                    for (int i = pipeline.size()-1; i >=1; --i) {
                        exit_code = pipeline[i].wait();
                    }
                }
                my_errno.set_code(exit_code, exit_code);

            }

            if (my_errno.get_code() != 0) {
                my_errno.get_description();
            }
            if (argc != 1 || !is_file) {
                std::cout << std::endl;
            }
        } catch (std::runtime_error &error) {
            std::cerr << error.what() << std::endl;
        } catch (...) {
            std::cout << "Syntax error!" << std::endl;
        }
    }
    return 0;
}

#pragma clang diagnostic pop