// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: http://www.viva64.com

#ifndef MYSHELL_MY_FUNCTIONS_H
#define MYSHELL_MY_FUNCTIONS_H

#include <vector>
#include <string>

#include "VariablesManager.h"

void mpwd();

void mcd_help();

void mpwd_help();

void mecho_help();

void mexit_help();

void mexport_help();

void mexit(int code = 0);

std::string get_current_directory();

void mcd(const std::string &new_path);

void mecho(const std::vector<std::string> &args);

void mexport(const std::vector<std::string> &args, VariablesManager &variablesManager);

int run_my_options(std::vector<std::string> &args, VariablesManager &variablesManager);

#endif //MYSHELL_MY_FUNCTIONS_H
