#include "myprint.h"
#include <iostream>

void print(const std::string &str, bool red) {
    if(red){
        std::cout << "\033[31"  << "m" << str;
    }else{
        std::cout << "\033[39"  << "m" << str;
    }
}
