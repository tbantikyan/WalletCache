#include "verification.hpp"
#include <algorithm>

auto ValidateCreditCardNumber(const std::string &number) -> bool {
    int length = number.size();
    if (length != 15 && length != 16) {
        return false;
    }

    // Luhn Algorithm
    int sum = 0;
    bool double_digit = false;
    for (int i = length - 1; i >= 0; i--) {
        int d = number[i] - '0';

        if (double_digit) {
            d = d * 2;
        }
        double_digit = !double_digit;

        sum += d / 10;
        sum += d % 10;
    }

    return (sum % 10 == 0);
}

auto ValidateInputAlnumOnly(std::string input) -> bool {
    if (input.empty()) {
        return false;
    }
    return std::all_of(input.begin(), input.end(), [](int x) { return isalnum(x) != 0 || x == 0x20; });
}

auto ValidateInputDigitsOnly(std::string input) -> bool {
    if (input.empty()) {
        return false;
    }
    return std::all_of(input.begin(), input.end(), ::isdigit);
}

auto ValidateInputInRange(const std::string &input, int lower, int upper) -> bool {
    try {
        int input_int = stoi(input);
        return (input_int >= lower && input_int <= upper);
    } catch (...) {
        return false;
    }
}

auto VerifyNewPassword(const std::string &password, const std::string &confirm_password) -> NewPasswordStatus {
    if (password.size() < MIN_PASSWORD_LENGTH) {
        return PASS_TOO_SHORT;
    }
    if (password.size() > MAX_PASSWORD_LENGTH) {
        return PASS_TOO_LONG;
    }
    if (password != confirm_password) {
        return PASS_NO_MATCH;
    }
    return PASS_VALID;
}
