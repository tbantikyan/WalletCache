#include "utils.hpp"

#include <cstdlib>
#include <filesystem>
#include <pwd.h>

#ifdef WIN32
#include <windows.h>
#else
#include <termios.h>
#include <unistd.h>
#endif

auto CheckFileExists(const std::string &path) -> bool { return std::filesystem::exists(path); }

void ClearScreen() {
#if defined _WIN32
    system("cls");
#elif defined(__LINUX__) || defined(__gnu_linux__) || defined(__linux__)
    system("clear");
#elif defined(__APPLE__)
    system("clear");
#endif
}

void EnableStdinEcho(bool enabled) {
#ifdef WIN32
    HANDLE hStdin = GetStdHandle(STD_INPUT_HANDLE);
    DWORD mode;
    GetConsoleMode(hStdin, &mode);

    if (!enable)
        mode &= ~ENABLE_ECHO_INPUT;
    else
        mode |= ENABLE_ECHO_INPUT;

    SetConsoleMode(hStdin, mode);
#else
    struct termios tty;
    tcgetattr(STDIN_FILENO, &tty);
    if (!enabled) {
        tty.c_lflag &= ~ECHO;
    } else {
        tty.c_lflag |= ECHO;
    }

    (void)tcsetattr(STDIN_FILENO, TCSANOW, &tty);
#endif
}

auto GetHomePath() -> std::string {
    std::string path;
#if defined(_WIN32)
    path = getenv("HOMEPATH");
#else
    uid_t uid = getuid();
    struct passwd pwd;
    char buf[1024];
    struct passwd *result;

    int ret = getpwuid_r(uid, &pwd, buf, sizeof(buf), &result);
    if (ret != 0) {
        return "";
    };

    path = std::string(pwd.pw_dir);
#endif

    return path;
}

auto GetFilePath(const std::string &dir, const std::string &file_name) -> std::string {
    std::string path;
#if defined(_WIN32)
    path = dir + "\\" + file_name;
#else
    path = dir + "/" + file_name;
#endif

    return path;
}
