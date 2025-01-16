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

    NewPasswordStatus valid_password;
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

    secure_cpy_str_to_buf(input_password, password);
    input_password.clear();
    input_confirm.clear();
}

int handle_new_profile(Store &store, UI &ui, bool profile_exists) {
    if (profile_exists) {
        store.delete_data();
    }

    unsigned char password[MAX_PASSWORD_LENGTH + 1];
    handle_password_setup(ui, password);
    ui.display_hashing();

    int res = store.init_new_store(password);
    memzero(password, MAX_PASSWORD_LENGTH + 1);
    return res;
}

int handle_login(Store &store, UI &ui) {
    string input_password;
    ui.prompt_login(input_password);

    unsigned char password[MAX_PASSWORD_LENGTH + 1];
    secure_cpy_str_to_buf(input_password, password);
    return store.load_store(password);
}

int main() {
    if (init_crypto() == -1) {
        cerr << "Failed to init crypto.\n";
        return -1;
    }

    UI ui = UI();
    Store store = Store();

    bool logged_in = false;
    string status_msg;
    while (!logged_in) {
        bool profile_exists = store.data_exists();

        UI::MenuOption selection = ui.start_menu(status_msg, profile_exists);
        if (check_profile_replacement(ui, profile_exists, selection) != 0) {
            continue;
        }

        switch (selection) {
        case UI::OPTION_EXIT:
            return 0;
        case UI::OPTION_NEW_PROFILE:
            if (handle_new_profile(store, ui, profile_exists) != 0) {
                status_msg = "ERR: Failed to initialize store for new profile\n";
            } else {
                status_msg = "Successfully created new profile!\n";
            }
            break;
        case UI::OPTION_LOGIN:
            switch (handle_login(store, ui)) {
            case Store::LOAD_STORE_VALID:
                logged_in = true;
                break;
            case Store::DATA_READ_ERROR:
                status_msg = "ERR: Login failed. Unable to read data file.\n";
                break;
            case Store::PWD_VERIFY_ERR:
                status_msg = "ERR: Login failed. Please ensure password is correct.\n";
                break;
            case Store::KEY_DERIVATION_ERROR:
                status_msg = "ERR: Failed to derive encryption key.\n";
                break;
            }
            break;
        }
    }
}
