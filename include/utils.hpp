#ifndef UTILS_HPP
#define UTILS_HPP

#include <string>

using namespace std;

bool CheckFileExists(string path);

void ClearScreen();

void EnableStdinEcho(bool enabled);

string GetHomePath();

string GetFilePath(string dir, string file_name);

#endif // UTILS_HPP
