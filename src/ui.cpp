#include "ui.hpp"
#include "utils.hpp"
#include "verification.hpp"

#include <iostream>

using namespace std;

UI::StartMenuOption UI::StartMenu(string status_msg, bool profile_exists) {
    ClearScreen();
    cout << status_msg;
    cout << "Welcome to WalletCache\n\n";
    cout << "Select an option below:\n";
    cout << "  (0): EXIT\n";
    cout << "  (1): CREATE NEW PROFILE\n";
    if (profile_exists) {
        cout << "  (2): LOGIN TO EXISTING PROFILE\n";
    }

    return (UI::StartMenuOption)GetSelection(0, profile_exists ? 2 : 1);
}

void UI::CreateProfileMenu(string status_msg, string &password, string &confirm_password) {
    ClearScreen();

    cout << status_msg;
    cout << "Enter a master password:\n";
    password = this->PromptInputMasked();
    cout << "Confirm master password:\n";
    confirm_password = this->PromptInputMasked();
}

UI::ProfileMenuOption UI::ProfileMenu(string status_msg) {
    ClearScreen();
    cout << status_msg;
    cout << "Welcome to WalletCache\n\n";
    cout << "Select an option below:\n";
    cout << "  (0): EXIT\n";
    cout << "  (1): LIST\n";
    cout << "  (2): ADD\n";
    cout << "  (3): DELETE\n";

    return (UI::ProfileMenuOption)GetSelection(0, 3);
}

void UI::CardsList(string cards_string) {
    ClearScreen();
    cout << cards_string;
}

void UI::DisplayHashing() { cout << "\nHashing...\n"; }

void UI::PromptCardCvv(string error_msg, string &cvv) {
    ClearScreen();

    cout << error_msg;
    cout << "Enter card CVV (or 0 to cancel):\n";
    cvv = this->PromptInput();
}

void UI::PromptCardMonth(string error_msg, string &month) {
    ClearScreen();

    cout << error_msg;
    cout << "Enter card expiration month [ex: 10 for October] (or 0 to cancel):\n";
    month = this->PromptInput();
}

void UI::PromptCardName(string error_msg, string &card_name) {
    ClearScreen();

    cout << error_msg;
    cout << "Optional: enter a name for the card [only letters or numbers] (or 0 "
            "to cancel):\n";
    card_name = this->PromptInput();
}

void UI::PromptCardNumber(string error_msg, string &card_number) {
    ClearScreen();

    cout << error_msg;
    cout << "Enter card number (or 0 to cancel):\n";
    card_number = this->PromptInput();
}

void UI::PromptCardYear(string error_msg, string &year) {
    ClearScreen();

    cout << error_msg;
    cout << "Enter card expiration year [ex: 2025] (or 0 to cancel):\n";
    year = this->PromptInput();
}

void UI::PromptLogin(string &password) {
    ClearScreen();

    cout << "Enter profile password:\n";
    password = this->PromptInputMasked();
}

bool UI::PromptConfirmation(string msg) {
    cout << "\n";
    cout << msg;
    cout << "(  0) Cancel\n";
    cout << "(  1) Confirm\n";

    return this->GetSelection(0, 1) == 1;
}

int UI::GetSelection(int lower, int upper) {
    string input_string;
    bool valid_input;
    do {
        input_string = this->PromptInput();
        valid_input = ValidateInputInRange(input_string, lower, upper);

        if (!valid_input) {
            cout << "Please input a number corresponding to available "
                    "options.\n";
        }
    } while (!valid_input);

    return stoi(input_string);
}

string UI::PromptInput() {
    cout << "> ";
    string input;
    getline(cin, input);
    cout << "\n";
    return input;
}

string UI::PromptInputMasked() {
    EnableStdinEcho(false);
    string input = this->PromptInput();
    EnableStdinEcho(true);
    return input;
}
