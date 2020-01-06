#include <fstream>
#include <sstream>
#include "my_file_reader.h"

std::vector<std::vector<std::string>> read_from_file(const std::string &filename) {
    std::ifstream file(filename);
    std::string word;
    std::string line;
    std::string trash;
    std::vector<std::string> file_in_memory;
    std::vector<std::vector<std::string>> commands;
    while (getline(file, line)) {
        std::istringstream str(line);
        while (str >> word) {
            if (word.at(0) == '#' && word.at(1) != '!') {
                break;
            } else {
                file_in_memory.push_back(word);
            }
        }
        commands.push_back(file_in_memory);
        file_in_memory.clear();
    }
    file.close();
    return commands;
}
