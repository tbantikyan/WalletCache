#include "ui.hpp"
#include "utils.hpp"
#include "verification.hpp"

#include <iostream>

using namespace std;

int UI::start_menu(string error_msg, bool profile_exists) {
start_menu_start:
    clear_screen();
    cout << error_msg;
    cout << "Welcome to WalletCache\n\n";
    cout << "Select an option below:\n";
    cout << "  (0): EXIT\n";
    cout << "  (1): CREATE NEW PROFILE\n";
    if (profile_exists) {
        cout << "  (2): LOGIN TO EXISTING PROFILE\n";
    }

    int input = get_selection(0, profile_exists ? 2 : 1);
    if (profile_exists && input == OPTION_NEW_PROFILE) {
        string msg = "Are you sure you want to create a new profile? This will "
                     "replace the existing profile.\n";
        if (!this->prompt_confirmation(msg)) {
            goto start_menu_start;
        }
    }
    return input;
}

int UI::create_profile_menu(unsigned char *password) {
    string input_password;
    string error_msg;
    int valid_password;
    do {
        clear_screen();

        cout << error_msg;
        cout << "Enter a master password\n";
        input_password = this->prompt_input_masked();
        cout << "Confirm master password\n";
        string input_confirm = this->prompt_input_masked();

        valid_password = verify_new_password(input_password, input_confirm);
        input_confirm.clear();
        switch (valid_password) {
        case PASS_TOO_SHORT:
            error_msg = "Password should be at least " +
                        to_string(MIN_PASSWORD_LENGTH) + " characters long.\n";
            break;
        case PASS_TOO_LONG:
            error_msg = "Password should be at most " +
                        to_string(MAX_PASSWORD_LENGTH) + " characters long.\n";
            break;
        case PASS_NO_MATCH:
            error_msg = "Passwords do not match.\n";
            break;
        default:
            break;
        }
    } while (valid_password != 0);

    memcpy(password, input_password.c_str(), input_password.size());
    password[input_password.size()] = '\0';
    input_password.clear();
    return 0;
}

int UI::get_selection(int lower, int upper) {
    string input_string;
    bool valid_input;
    do {
        input_string = this->prompt_input();
        valid_input = validate_input_int(input_string, lower, upper);

        if (!valid_input) {
            cout << "Please input a number corresponding to available "
                    "options.\n";
        }
    } while (!valid_input);

    return stoi(input_string);
}

bool UI::prompt_confirmation(string msg) {
    cout << "\n";
    cout << msg;
    cout << "(  0) Cancel\n";
    cout << "(  1) Confirm\n";

    return get_selection(0, 1) == 1;
}

string UI::prompt_input() {
    cout << "> ";
    string input;
    cin >> input;
    cout << "\n";
    return input;
}

string UI::prompt_input_masked() {
    enable_stdin_echo(false);
    string input = this->prompt_input();
    enable_stdin_echo(true);
    return input;
}
