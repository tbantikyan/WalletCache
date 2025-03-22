#include "ui.hpp"
#include "utils.hpp"
#include "verification.hpp"

#include <iostream>
#include <unordered_map>

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

    return static_cast<UI::StartMenuOption>(this->GetSelection(0, profile_exists ? 2 : 1));
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

    return static_cast<UI::ProfileMenuOption>(this->GetSelection(0, 3));
}

auto UI::ListCardsMenu(const std::vector<std::pair<uint32_t, std::string>> &cards_list) -> int {
    ClearScreen();

    auto choice_mapping = std::unordered_map<int, uint32_t>();
    std::cout << UIStrings::LIST_CARDS_RETURN;

    int opt = 1;
    for (const std::pair<uint32_t, std::string> &card_item : cards_list) {
        std::cout << "[" << opt << "] " << card_item.second << "\n";
        choice_mapping.insert(std::make_pair(opt, card_item.first));
        opt++;
    }

    int selection = this->GetSelection(0, static_cast<int>(cards_list.size()));
    if (selection == 0) {
        return -1;
    }
    return static_cast<int>(choice_mapping.at(selection));
}

auto UI::CardInfoMenu(const std::vector<std::pair<std::string, std::string>> &card_fields, uint32_t *selected_field,
                      bool fields_visible) -> UI::CardInfoMenuOption {
    ClearScreen();

    auto choice_mapping = std::unordered_map<int, uint32_t>();
    std::cout << UIStrings::CARD_INFO_RETURN;
    std::cout << UIStrings::CARD_INFO_DELETE;
    std::cout << UIStrings::CARD_INFO_TOGGLE_VISIBILITY;

    std::cout << UIStrings::CARD_INFO_FIELDS_PROMPT;
    int opt = 3;
    auto fields_size = static_cast<uint32_t>(card_fields.size());
    for (uint32_t i = 0; i < fields_size; ++i) {
        const std::pair<std::string, std::string> &field = card_fields[i];
        std::string field_value =
            (!fields_visible && field.first != UIStrings::CARD_NAME_LABEL) ? UIStrings::HIDDEN_FIELD : field.second;
        std::cout << "[" << opt << "] " << field.first << field_value << "\n";
        choice_mapping.insert({opt, i});
        opt++;
    }

    int selection = this->GetSelection(0, static_cast<int>(card_fields.size()) + 1);
    if (selection == 0) {
        return OPT_CARD_RETURN;
    }
    if (selection == 1) {
        return OPT_CARD_DELETE;
    }
    if (selection == 2) {
        return OPT_CARD_TOGGLE_VISIBLE;
    }
    *selected_field = static_cast<int>(choice_mapping.at(selection));
    return OPT_CARD_RETURN;
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
