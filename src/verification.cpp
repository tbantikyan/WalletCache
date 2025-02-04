#include "verification.hpp"
#include <algorithm>

bool ValidateCreditCardNumber(string number) {
    int length = number.size();
    if (length != 15 && length != 16) {
        return false;
    }

    // Luhn Algorithm
    int sum = 0;
    bool double_digit = false;
    for (int i = length - 1; i >= 0; i--) {
        int d = number[i] - '0';

        if (double_digit == true)
            d = d * 2;
        double_digit = !double_digit;

        sum += d / 10;
        sum += d % 10;
    }

    return (sum % 10 == 0);
}

bool ValidateInputAlnumOnly(string input) {
    if (input.size() == 0) {
        return false;
    }
    return std::all_of(input.begin(), input.end(), isalnum); 
}

bool ValidateInputDigitsOnly(string input) {
    if (input.size() == 0) {
        return false;
    }
    return std::all_of(input.begin(), input.end(), ::isdigit); 
}

bool ValidateInputInRange(string input, int lower, int upper) {
    try {
        int input_int = stoi(input);
        return (input_int >= lower && input_int <= upper);
    } catch (...) {
        return false;
    }
}

NewPasswordStatus VerifyNewPassword(string password, string confirm_password) {
    if (password.size() < MIN_PASSWORD_LENGTH) {
        return PASS_TOO_SHORT;
    } else if (password.size() > MAX_PASSWORD_LENGTH) {
        return PASS_TOO_LONG;
    } else if (password != confirm_password) {
        return PASS_NO_MATCH;
    }
    return PASS_VALID;
}
