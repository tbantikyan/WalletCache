#ifndef VERIFICATION_HPP
#define VERIFICATION_HPP

#include <string>
using namespace std;

#define MIN_PASSWORD_LENGTH 8
#define MAX_PASSWORD_LENGTH 128

enum PasswordStatus{
    PASS_VALID = 0,
    PASS_NO_MATCH,
    PASS_TOO_SHORT,
    PASS_TOO_LONG,
};

PasswordStatus verify_new_password(string password, string confirm_password);

#endif // VERIFICATION_HPP
