#include "utils.hpp"

#include <filesystem>
#include <pwd.h>
#include <stdlib.h>

#ifdef WIN32
#include <windows.h>
#else
#include <termios.h>
#include <unistd.h>
#endif

using namespace std;

bool check_file_exists(string path) {
    return filesystem::exists(path);
}

void clear_screen() {
#if defined _WIN32
    system("cls");
#elif defined(__LINUX__) || defined(__gnu_linux__) || defined(__linux__)
    system("clear");
#elif defined(__APPLE__)
    system("clear");
#endif
}

void enable_stdin_echo(bool enabled) {
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
    if (!enabled)
        tty.c_lflag &= ~ECHO;
    else
        tty.c_lflag |= ECHO;

    (void)tcsetattr(STDIN_FILENO, TCSANOW, &tty);
#endif
}

string get_home_path() {
    string path;
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

    path = string(pwd.pw_dir);
#endif

    return path;
}

string get_file_path(string dir, string file_name) {
    string path;
#if defined(_WIN32)
    path = dir + "\\" + file_name;
#else
    path = dir + "/" + file_name;
#endif

    return path;
}

bool validate_input_int(string input, int lower, int upper) {
    try {
        int input_int = stoi(input);
        return (input_int >= lower && input_int <= upper);
    } catch (...) {
        return false;
    }
}
