#include "ui.hpp"
#include "utils.hpp"
#include "verification.hpp"

#include <iostream>

using namespace std;

UI::MenuOption UI::start_menu(string error_msg, bool profile_exists) {
    clear_screen();
    cout << error_msg;
    cout << "Welcome to WalletCache\n\n";
    cout << "Select an option below:\n";
    cout << "  (0): EXIT\n";
    cout << "  (1): CREATE NEW PROFILE\n";
    if (profile_exists) {
        cout << "  (2): LOGIN TO EXISTING PROFILE\n";
    }

    return (UI::MenuOption)get_selection(0, profile_exists ? 2 : 1);
}

void UI::create_profile_menu(string error_msg, string &password, string &confirm_password) {
    clear_screen();

    cout << error_msg;
    cout << "Enter a master password:\n";
    password = this->prompt_input_masked();
    cout << "Confirm master password:\n";
    confirm_password = this->prompt_input_masked();
}

void UI::display_hashing() {
    cout << "\nHashing...\n";
}

void UI::prompt_login(string &password) {
    clear_screen();

    cout << "Enter profile password:\n";
    password = this->prompt_input_masked();
}

bool UI::prompt_confirmation(string msg) {
    cout << "\n";
    cout << msg;
    cout << "(  0) Cancel\n";
    cout << "(  1) Confirm\n";

    return this->get_selection(0, 1) == 1;
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
