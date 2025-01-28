#include "verification.hpp"
#include <algorithm>

bool validate_credit_card_number(string number) {
    int length = number.size();
    if (length != 16) {
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

bool validate_input_alnum_only(string input) {
    if (input.size() == 0) {
        return false;
    }
    return std::all_of(input.begin(), input.end(), isalnum); 
}

bool validate_input_digits_only(string input) {
    if (input.size() == 0) {
        return false;
    }
    return std::all_of(input.begin(), input.end(), ::isdigit); 
}

bool validate_input_in_range(string input, int lower, int upper) {
    try {
        int input_int = stoi(input);
        return (input_int >= lower && input_int <= upper);
    } catch (...) {
        return false;
    }
}

NewPasswordStatus verify_new_password(string password, string confirm_password) {
    if (password.size() < MIN_PASSWORD_LENGTH) {
        return PASS_TOO_SHORT;
    } else if (password.size() > MAX_PASSWORD_LENGTH) {
        return PASS_TOO_LONG;
    } else if (password != confirm_password) {
        return PASS_NO_MATCH;
    }
    return PASS_VALID;
}
