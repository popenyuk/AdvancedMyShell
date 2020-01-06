// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: http://www.viva64.com

#ifndef MYSHELL_WILDCARD_PARSER_H
#define MYSHELL_WILDCARD_PARSER_H

#include <string>
#include <vector>
#include "File.h"

int is_wildcard(const std::string &command);

std::string convert_pattern(const std::string &pattern);

std::string from_wildcard(const std::string &wildcard, bool path);

std::vector<File> get_matches(const std::vector<File>& files, const std::string &pattern);

std::vector<std::string> get_matches(const std::vector<std::string> &files, const std::string &pattern);

#endif //MYSHELL_WILDCARD_PARSER_H
