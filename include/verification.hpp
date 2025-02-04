#ifndef VERIFICATION_HPP
#define VERIFICATION_HPP

#include <string>

#define MIN_PASSWORD_LENGTH 8
#define MAX_PASSWORD_LENGTH 128

enum NewPasswordStatus {
    PASS_VALID = 0,
    PASS_NO_MATCH,
    PASS_TOO_SHORT,
    PASS_TOO_LONG,
};

auto ValidateCreditCardNumber(const std::string &number) -> bool;

auto ValidateInputAlnumOnly(std::string input) -> bool;
auto ValidateInputDigitsOnly(std::string input) -> bool;
auto ValidateInputInRange(const std::string &input, int lower, int upper) -> bool;

auto VerifyNewPassword(const std::string &password, const std::string &confirm_password) -> NewPasswordStatus;

#endif // VERIFICATION_HPP
