#include "ui.hpp"
#include "utils.hpp"
#include "verification.hpp"

#include <iostream>

auto UI::StartMenu(const std::string &status_msg, bool profile_exists) -> UI::StartMenuOption {
    ClearScreen();
    std::cout << status_msg;
    std::cout << UIStrings::WELCOME; 
    std::cout << UIStrings::SELECT_OPT; 
    std::cout << UIStrings::START_MENU_EXIT;
    std::cout << UIStrings::START_MENU_CREATE_PROFILE;
    if (profile_exists) {
        std::cout << UIStrings::START_MENU_LOGIN_PROFILE;
    }

    return static_cast<UI::StartMenuOption>(GetSelection(0, profile_exists ? 2 : 1));
}

void UI::CreateProfileMenu(const std::string &status_msg, std::string &password, std::string &confirm_password) {
    ClearScreen();

    std::cout << status_msg;
    std::cout << UIStrings::CREATE_PROFILE_MENU_PASSWORD;
    password = this->PromptInputMasked();
    std::cout << UIStrings::CREATE_PROFILE_MENU_CONFIRM_PASSWORD;
    confirm_password = this->PromptInputMasked();
}

auto UI::ProfileMenu(const std::string &status_msg) -> UI::ProfileMenuOption {
    ClearScreen();
    std::cout << status_msg;
    std::cout << UIStrings::WELCOME;
    std::cout << UIStrings::SELECT_OPT;
    std::cout << UIStrings::PROFILE_MENU_EXIT;
    std::cout << UIStrings::PROFILE_MENU_LIST;
    std::cout << UIStrings::PROFILE_MENU_ADD;
    std::cout << UIStrings::PROFILE_MENU_DELETE;

    return static_cast<UI::ProfileMenuOption>(GetSelection(0, 3));
}

void UI::CardsList(const std::string &cards_string) {
    ClearScreen();
    std::cout << cards_string;
}

void UI::DisplayHashing() { std::cout << UIStrings::HASHING; }

void UI::PromptCardCvv(const std::string &status_msg, std::string &cvv) {
    ClearScreen();

    std::cout << status_msg;
    std::cout << UIStrings::CARD_CVV_PROMPT;
    cvv = this->PromptInput();
}

void UI::PromptCardMonth(const std::string &status_msg, std::string &month) {
    ClearScreen();

    std::cout << status_msg;
    std::cout << UIStrings::CARD_MONTH_PROMPT;
    month = this->PromptInput();
}

void UI::PromptCardName(const std::string &status_msg, std::string &card_name) {
    ClearScreen();

    std::cout << status_msg;
    std::cout << UIStrings::CARD_NAME_PROMPT;
    card_name = this->PromptInput();
}

void UI::PromptCardNumber(const std::string &status_msg, std::string &card_number) {
    ClearScreen();

    std::cout << status_msg;
    std::cout << UIStrings::CARD_NUMBER_PROMPT;
    card_number = this->PromptInput();
}

void UI::PromptCardYear(const std::string &status_msg, std::string &year) {
    ClearScreen();

    std::cout << status_msg;
    std::cout << UIStrings::CARD_YEAR_PROMPT;
    year = this->PromptInput();
}

void UI::PromptLogin(std::string &password) {
    ClearScreen();

    std::cout << UIStrings::PASSWORD_PROMPT;
    password = this->PromptInputMasked();
}

auto UI::PromptConfirmation(const std::string &msg) -> bool {
    std::cout << "\n";
    std::cout << msg;
    std::cout << UIStrings::CONFIRMATION_CANCEL;
    std::cout << UIStrings::CONFIRMATION_CONFIRM;

    return this->GetSelection(0, 1) == 1;
}

auto UI::GetSelection(int lower, int upper) -> int {
    std::string input_string;
    bool valid_input;
    do {
        input_string = this->PromptInput();
        valid_input = ValidateInputInRange(input_string, lower, upper);

        if (!valid_input) {
            std::cout << UIStrings::REQUEST_VALID_INPUT;
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
