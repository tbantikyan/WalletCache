#include "utils.hpp"
#include <stdlib.h>

void clear_screen() {
#if defined _WIN32
    system("cls");
#elif defined(__LINUX__) || defined(__gnu_linux__) || defined(__linux__)
    system("clear");
#elif defined(__APPLE__)
    system("clear");
#endif
}

bool validate_input_int(std::string input, int lower, int upper) {
    try {
        int input_int = std::stoi(input);
        return (input_int >= lower && input_int <= upper);
    } catch (...) {
        return false;
    }
}
