#include "ui.hpp"
#include "utils.hpp"
#include "verification.hpp"

#include <iostream>

using namespace std;

UI::StartMenuOption UI::start_menu(string status_msg, bool profile_exists) {
    clear_screen();
    cout << status_msg;
    cout << "Welcome to WalletCache\n\n";
    cout << "Select an option below:\n";
    cout << "  (0): EXIT\n";
    cout << "  (1): CREATE NEW PROFILE\n";
    if (profile_exists) {
        cout << "  (2): LOGIN TO EXISTING PROFILE\n";
    }

    return (UI::StartMenuOption)get_selection(0, profile_exists ? 2 : 1);
}

void UI::create_profile_menu(string status_msg, string &password, string &confirm_password) {
    clear_screen();

    cout << status_msg;
    cout << "Enter a master password:\n";
    password = this->prompt_input_masked();
    cout << "Confirm master password:\n";
    confirm_password = this->prompt_input_masked();
}

UI::ProfileMenuOption UI::profile_menu(string status_msg) {
    clear_screen();
    cout << status_msg;
    cout << "Welcome to WalletCache\n\n";
    cout << "Select an option below:\n";
    cout << "  (0): EXIT\n";
    cout << "  (1): LIST\n";
    cout << "  (2): ADD\n";
    cout << "  (3): DELETE\n";

    return (UI::ProfileMenuOption)get_selection(0, 3);
}

void UI::cards_list(string cards_string) {
    clear_screen();
    cout << cards_string;
}

void UI::display_hashing() { cout << "\nHashing...\n"; }

void UI::prompt_card_cvv(string error_msg, string &cvv) {
    clear_screen();

    cout << error_msg;
    cout << "Enter card CVV (or 0 to cancel):\n";
    cvv = this->prompt_input();
}

void UI::prompt_card_month(string error_msg, string &month) {
    clear_screen();

    cout << error_msg;
    cout << "Enter card expiration month [ex: 10 for October] (or 0 to cancel):\n";
    month = this->prompt_input();
}

void UI::prompt_card_name(string error_msg, string &card_name) {
    clear_screen();
    
    cout << error_msg;
    cout << "Optional: enter a name for the card [only letters or numbers] (or 0 to cancel):\n";
    card_name = this->prompt_input();
}

void UI::prompt_card_number(string error_msg, string &card_number) {
    clear_screen();
    
    cout << error_msg;
    cout << "Enter card number (or 0 to cancel):\n";
    card_number = this->prompt_input();
}

void UI::prompt_card_year(string error_msg, string &year) {
    clear_screen();

    cout << error_msg;
    cout << "Enter card expiration year [ex: 2025] (or 0 to cancel):\n";
    year = this->prompt_input();
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
        valid_input = validate_input_in_range(input_string, lower, upper);

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
    getline(cin, input);
    cout << "\n";
    return input;
}

string UI::prompt_input_masked() {
    enable_stdin_echo(false);
    string input = this->prompt_input();
    enable_stdin_echo(true);
    return input;
}
