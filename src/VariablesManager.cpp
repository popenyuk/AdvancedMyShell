// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: http://www.viva64.com

#include "VariablesManager.h"

void VariablesManager::setGlobalVariable(const std::string &name, std::string data) {
    if ((data[0] == '\'' && data[data.size() - 1] == '\'') || (data[0] == '"' && data[data.size() - 1] == '"')) {
        localVariables[name] = data.substr(1, data.size() - 2);
        globalVariables[name] = data.substr(1, data.size() - 2);
    } else {
        localVariables[name] = data;
        globalVariables[name] = data;
    }
    this->updatePath();

}

std::string VariablesManager::getGlobalVariable(const std::string &name) {
    return globalVariables[name];
}

void VariablesManager::setLocalVariable(const std::string &name, std::string data) {
    if ((data[0] == '\'' && data[data.size() - 1] == '\'') || (data[0] == '"' && data[data.size() - 1] == '"')) {
        localVariables[name] = data.substr(1, data.size() - 2);
    } else {
        localVariables[name] = data;
    }
    this->updatePath();

}

std::string VariablesManager::getLocalVariable(const std::string &name) {
    return localVariables[name];
}

std::string VariablesManager::getAllGlobalVariable() const {
    std::string str;
    for (auto &key:globalVariables) {
        str.insert(str.size(), key.second);
    }
    return str;
}

std::string VariablesManager::getAllLocalVariable() const {
    std::string str;
    for (auto &key:localVariables) {
        str.insert(str.size(), key.second);
        str.insert(str.size(), ":");
    }
    return str;
}


void VariablesManager::updatePath() {
    std::size_t current, previous = 0;
    auto pathStr = this->getLocalVariable("PATH");
    current = pathStr.find(':');
    while (current != std::string::npos) {
        path.push_back(pathStr.substr(previous, current - previous)  + "/");
        previous = current + 1;
        current = pathStr.find(':', previous);
    }
    path.push_back(pathStr.substr(previous, current - previous)  + "/");

}

char **VariablesManager::getENVPRepr() {
    if (globalVariables.empty())
        return nullptr;

    char **myEnvp = new char *[globalVariables.size() + 1];
    myEnvp[globalVariables.size()] = nullptr;

    int addr = 0;
    for (auto &iter : globalVariables) {
        auto row = iter.first + "=" + iter.second;
        auto res = new char[row.size() + 1];

        std::copy(&row[0], &row[0] + row.size() + 1, res);
        myEnvp[addr++] = res;
    }



};