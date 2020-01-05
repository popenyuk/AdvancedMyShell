// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: http://www.viva64.com

#include <regex>
#include <vector>
#include <iostream>
#include <algorithm>

#include "wildcard_parser.h"

using std::vector;


std::string convert_pattern(const std::string &pattern) {
    std::string reg_pattern;
    for (auto s: pattern) {
        if (s == '*')
            reg_pattern += ".*";
        else if (s == '.')
            reg_pattern += "\\.";
        else if (s == '?')
            reg_pattern += '.';
        else {
            reg_pattern += s;
        }
    }
    return reg_pattern;
}


void remove_chars_from_string(std::string &str, char *charsToRemove) {
    for (unsigned int i = 0; i < strlen(charsToRemove); ++i) {
        str.erase(remove(str.begin(), str.end(), charsToRemove[i]), str.end());
    }
}


vector<File> get_matches(const vector<File>& files, const std::string &pattern) {
    vector<File> final{};
    std::regex rp(convert_pattern(pattern));

    for (const auto &f: files) {
        int ind = f.getName().find_last_of('/');
        std::string c = f.getName();
        if (ind != std::string::npos)
            c = f.getName().substr(ind + 1);

        if (std::regex_match(c, rp)) {
            final.push_back(f);
        }
    }

    return final;
}


// bool path -> indicates what you need to subtract from wildcard (the path
// or the wildcard itself)
std::string from_wildcard(const std::string &wildcard, bool path) {
    auto ind = wildcard.find_last_of('/');
    if (ind == std::string::npos) {
        if (path)
            return "";
        return wildcard;
    }

    std::string res;
    if (path) {
        res = wildcard.substr(0, ind + 1);
        if (res[0] != '/' && res[0] != '.')
            res = res.insert(0, "/");
        if (res[res.length() - 1] != '/')
            res += '/';
    } else {
        res = wildcard.substr(ind);
        if (res[0] == '/')
            res = res.substr(1);
    }

    return res;
}


int is_wildcard(const std::string &command) {
    if (command.empty())
        return 0;

    auto wild = from_wildcard(command, false);

    size_t stars = std::count(wild.begin(), wild.end(), '*');
    size_t open_brackets = std::count(wild.begin(), wild.end(), '[');
    size_t close_brackets = std::count(wild.begin(), wild.end(), ']');
    size_t question_mark = std::count(wild.begin(), wild.end(), '?');

    if (open_brackets == close_brackets && open_brackets != 0) return 1;
    if (stars) return 1;
    if (question_mark) return 1;

    if (open_brackets != close_brackets)
        throw std::invalid_argument("The syntax problem in a wildcard was detected.");

    return 0;
}