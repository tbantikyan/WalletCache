#include "verification.hpp"

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
