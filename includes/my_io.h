#ifndef ADVANCEDMYSHELL_MY_IO_H
#define ADVANCEDMYSHELL_MY_IO_H

#include <vector>
#include <string>

int change_d(std::vector<std::string> &args);

void return_d(const std::vector<int>& fds);

int read_input_from_file(std::vector<std::string> &args);

#endif //ADVANCEDMYSHELL_MY_IO_H
