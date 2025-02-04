#include "ui.hpp"
#include "utils.hpp"
#include "verification.hpp"

#include <iostream>

auto UI::StartMenu(const std::string &status_msg, bool profile_exists) -> UI::StartMenuOption {
    ClearScreen();
    std::cout << status_msg;
    std::cout << "Welcome to WalletCache\n\n";
    std::cout << "Select an option below:\n";
    std::cout << "  (0): EXIT\n";
    std::cout << "  (1): CREATE NEW PROFILE\n";
    if (profile_exists) {
        std::cout << "  (2): LOGIN TO EXISTING PROFILE\n";
    }

    return static_cast<UI::StartMenuOption>(GetSelection(0, profile_exists ? 2 : 1));
}

void UI::CreateProfileMenu(const std::string &status_msg, std::string &password, std::string &confirm_password) {
    ClearScreen();

    std::cout << status_msg;
    std::cout << "Enter a master password:\n";
    password = this->PromptInputMasked();
    std::cout << "Confirm master password:\n";
    confirm_password = this->PromptInputMasked();
}

auto UI::ProfileMenu(const std::string &status_msg) -> UI::ProfileMenuOption {
    ClearScreen();
    std::cout << status_msg;
    std::cout << "Welcome to WalletCache\n\n";
    std::cout << "Select an option below:\n";
    std::cout << "  (0): EXIT\n";
    std::cout << "  (1): LIST\n";
    std::cout << "  (2): ADD\n";
    std::cout << "  (3): DELETE\n";

    return static_cast<UI::ProfileMenuOption>(GetSelection(0, 3));
}

void UI::CardsList(const std::string &cards_string) {
    ClearScreen();
    std::cout << cards_string;
}

void UI::DisplayHashing() { std::cout << "\nHashing...\n"; }

void UI::PromptCardCvv(const std::string &status_msg, std::string &cvv) {
    ClearScreen();

    std::cout << status_msg;
    std::cout << "Enter card CVV (or 0 to cancel):\n";
    cvv = this->PromptInput();
}

void UI::PromptCardMonth(const std::string &status_msg, std::string &month) {
    ClearScreen();

    std::cout << status_msg;
    std::cout << "Enter card expiration month [ex: 10 for October] (or 0 "
                 "to cancel):\n";
    month = this->PromptInput();
}

void UI::PromptCardName(const std::string &status_msg, std::string &card_name) {
    ClearScreen();

    std::cout << status_msg;
    std::cout << "Optional: enter a name for the card [only letters or "
                 "numbers] (or 0 "
                 "to cancel):\n";
    card_name = this->PromptInput();
}

void UI::PromptCardNumber(const std::string &status_msg, std::string &card_number) {
    ClearScreen();

    std::cout << status_msg;
    std::cout << "Enter card number (or 0 to cancel):\n";
    card_number = this->PromptInput();
}

void UI::PromptCardYear(const std::string &status_msg, std::string &year) {
    ClearScreen();

    std::cout << status_msg;
    std::cout << "Enter card expiration year [ex: 2025] (or 0 to cancel):\n";
    year = this->PromptInput();
}

void UI::PromptLogin(std::string &password) {
    ClearScreen();

    std::cout << "Enter profile password:\n";
    password = this->PromptInputMasked();
}

auto UI::PromptConfirmation(const std::string &msg) -> bool {
    std::cout << "\n";
    std::cout << msg;
    std::cout << "(  0) Cancel\n";
    std::cout << "(  1) Confirm\n";

    return this->GetSelection(0, 1) == 1;
}

auto UI::GetSelection(int lower, int upper) -> int {
    std::string input_string;
    bool valid_input;
    do {
        input_string = this->PromptInput();
        valid_input = ValidateInputInRange(input_string, lower, upper);

        if (!valid_input) {
            std::cout << "Please input a number corresponding to available "
                         "options.\n";
        }
    } while (!valid_input);

    return std::stoi(input_string);
}

auto UI::PromptInput() -> std::string {
    std::cout << "> ";
    std::string input;
    std::getline(std::cin, input);
    std::cout << "\n";
    return input;
}

auto UI::PromptInputMasked() -> std::string {
    EnableStdinEcho(false);
    std::string input = this->PromptInput();
    EnableStdinEcho(true);
    return input;
}
