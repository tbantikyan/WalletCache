#ifndef UTILS_HPP
#define UTILS_HPP

#include <string>

auto CheckFileExists(const std::string &path) -> bool;

void ClearScreen();

void EnableStdinEcho(bool enabled);

auto GetHomePath() -> std::string;

auto GetFilePath(const std::string &dir, const std::string &file_name) -> std::string;

void CopyToClipboard(const std::string &str);
#endif // UTILS_HPP
