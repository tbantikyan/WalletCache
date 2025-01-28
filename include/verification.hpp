#ifndef VERIFICATION_HPP
#define VERIFICATION_HPP

#include <string>
using namespace std;

#define MIN_PASSWORD_LENGTH 8
#define MAX_PASSWORD_LENGTH 128

enum NewPasswordStatus {
    PASS_VALID = 0,
    PASS_NO_MATCH,
    PASS_TOO_SHORT,
    PASS_TOO_LONG,
};

bool validate_credit_card_number(string number);

bool validate_input_alnum_only(string input);
bool validate_input_digits_only(string input);
bool validate_input_in_range(string input, int lower, int upper);

NewPasswordStatus verify_new_password(string password, string confirm_password);

#endif // VERIFICATION_HPP
