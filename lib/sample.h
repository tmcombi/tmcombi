#ifndef LIB_SAMPLE_H_
#define LIB_SAMPLE_H_

#include <iostream>

void print_localtime();

int add( int i, int j ) { return i+j; }

void print_localtime() {
    std::time_t result = std::time(nullptr);
    std::cout << std::asctime(std::localtime(&result));
}

#endif
