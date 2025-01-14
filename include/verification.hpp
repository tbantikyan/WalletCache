#ifndef VERIFICATION_HPP
#define VERIFICATION_HPP

#include <string>
using namespace std;

#define MIN_PASSWORD_LENGTH 8
#define MAX_PASSWORD_LENGTH 128

enum PasswordErr{
    PASS_NO_MATCH = 1,
    PASS_TOO_SHORT,
    PASS_TOO_LONG,
};

int verify_new_password(string password, string confirm_password);

#endif // VERIFICATION_HPP
