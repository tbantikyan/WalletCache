#ifndef UTILS_HPP
#define UTILS_HPP

#include <string>

using namespace std;

bool check_file_exists(string path);

void clear_screen();

void enable_stdin_echo(bool enabled);

string get_home_path();

string get_file_path(string dir, string file_name);

#endif // UTILS_HPP
