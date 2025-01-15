#include "crypto.hpp"
#include "store.hpp"
#include "ui.hpp"
#include "verification.hpp"

#include <iostream>

using namespace std;

int check_profile_replacement(UI ui, bool profile_exists, UI::MenuOption input) {
    if (profile_exists && input == UI::OPTION_NEW_PROFILE) {
        string msg = "Are you sure you want to create a new profile? This will "
                     "replace the existing profile.\n";
        if (!ui.prompt_confirmation(msg)) {
            return -1;
        }
    }
    return 0;
}

void handle_password_setup(UI &ui, unsigned char *password) {
    string error_msg;
    string input_password;
    string input_confirm;

    PasswordStatus valid_password;
    do {
        ui.create_profile_menu(error_msg, input_password, input_confirm);

        valid_password = verify_new_password(input_password, input_confirm);
        switch (valid_password) {
        case PASS_TOO_SHORT:
            error_msg = "Password should be at least " + to_string(MIN_PASSWORD_LENGTH) + " characters long.\n";
            break;
        case PASS_TOO_LONG:
            error_msg = "Password should be at most " + to_string(MAX_PASSWORD_LENGTH) + " characters long.\n";
            break;
        case PASS_NO_MATCH:
            error_msg = "Passwords do not match.\n";
            break;
        case PASS_VALID:
            break;
        }
    } while (valid_password != PASS_VALID);

    memcpy(password, input_password.c_str(), input_password.size());
    password[input_password.size()] = '\0';
    input_password.clear();
    input_confirm.clear();
}

int handle_new_profile(Store &store, UI &ui) {
    unsigned char password[MAX_PASSWORD_LENGTH + 1];
    handle_password_setup(ui, password);
    int res = store.init_new_store(password);
    memzero(password, MAX_PASSWORD_LENGTH + 1);
    return res;
}

int main() {
    if (init_crypto() == -1) {
        cerr << "Failed to init crypto.\n";
        return -1;
    }

    UI ui = UI();
    Store store = Store();

    string status_msg;
    while (true) {
        bool profile_exists = store.data_exists();

        UI::MenuOption selection = ui.start_menu(status_msg, profile_exists);
        if (check_profile_replacement(ui, profile_exists, selection) != 0) {
            continue;
        }

        switch (selection) {
        case UI::OPTION_EXIT:
            return 0;
        case UI::OPTION_NEW_PROFILE:
            store.delete_data();

            if (handle_new_profile(store, ui) != 0) {
                status_msg = "ERR: Failed to initialize store for new profile\n";
            } else {
                status_msg = "Successfully created new profile!\n";
            }
            break;
        case UI::OPTION_LOGIN:
            break;
        }
    }
}
