#ifndef UI_HPP
#define UI_HPP

#include <cstdint>
#include <string>
#include <unordered_map>
#include <vector>

struct UIStrings {
    static const inline std::string WELCOME = "Welcome to WalletCache\n\n";
    static const inline std::string SELECT_OPT = "Select an option below:\n";

    static const inline std::string START_MENU_EXIT = "[0]: EXIT\n";
    static const inline std::string START_MENU_CREATE_PROFILE = "[1]: CREATE NEW PROFILE\n";
    static const inline std::string START_MENU_LOGIN_PROFILE = "[2]: LOGIN TO EXISTING PROFILE\n";

    static const inline std::string CREATE_PROFILE_MENU_PASSWORD = "Enter a master password:\n";
    static const inline std::string CREATE_PROFILE_MENU_CONFIRM_PASSWORD = "Confirm your master password:\n";

    static const inline std::string PROFILE_MENU_EXIT = "[0]: EXIT\n";
    static const inline std::string PROFILE_MENU_LIST = "[1]: LIST\n";
    static const inline std::string PROFILE_MENU_ADD = "[2]: ADD\n";
    static const inline std::string PROFILE_MENU_DELETE = "[3]: DELETE\n";

    static const inline std::string HASHING = "\nHashing...\n";

    static const inline std::string CARD_CVV_PROMPT = "Enter card cvv (or 0 to cancel):\n";
    static const inline std::string CARD_MONTH_PROMPT =
        "Enter card expiration month [Ex: 10 for october] (or 0 to cancel):\n";
    static const inline std::string CARD_NAME_PROMPT =
        "Optional: enter a name for the card using only letters, numbers, and "
        "spaces (or 0 to cancel):\n";
    static const inline std::string CARD_NUMBER_PROMPT = "Enter card number (or 0 to cancel):\n";
    static const inline std::string CARD_YEAR_PROMPT = "Enter card expiration year [Ex: 2025] (or 0 to cancel):\n";

    static const inline std::string PASSWORD_PROMPT = "Enter the profile master password:\n";

    static const inline std::string CONFIRMATION_CANCEL = "[0] CANCEL\n";
    static const inline std::string CONFIRMATION_CONFIRM = "[1] CONFIRM\n";

    static const inline std::string REQUEST_VALID_INPUT = "Please input a number corresponding to available options.\n";

    static const inline std::string LIST_CARDS_RETURN = "[0] RETURN\n";

    static const inline std::string CARD_INFO_RETURN = "[0] RETURN\n";
    static const inline std::string CARD_INFO_DELETE = "[1] DELETE CARD\n";
    static const inline std::string CARD_INFO_TOGGLE_VISIBILITY = "[2] TOGGLE FIELD VISIBLITY\n";
    static const inline std::string CARD_INFO_FIELDS_PROMPT =
        "\nCard fields (input corresponding number to copy field to "
        "clipboard):\n";

    static const inline std::string CARD_NAME_LABEL = "Name: ";
    static const inline std::string CARD_NUMBER_LABEL = "Card Number: ";
    static const inline std::string CARD_CVV_LABEL = "CVV: ";
    static const inline std::string CARD_MONTH_LABEL = "Expiration Month: ";
    static const inline std::string CARD_YEAR_LABEL = "Expiration Year: ";

    static const inline std::string HIDDEN_FIELD = "•••••";

    static const inline std::string DELETE_CARD_MESSAGE = "Select a card to delete:\n";
    static const inline std::string DELETE_CARD_RETURN = "[0] RETURN\n";
    static const inline std::string DELETE_CARD_CONFIRM_SELECTION = "Are you sure you wnat to delete this card?\n";
};

class UI {
    friend class UITest;

  public:
    enum StartMenuOption {
        OPT_START_EXIT = 0,
        OPT_START_NEW_PROFILE,
        OPT_START_LOGIN,
    };
    enum ProfileMenuOption {
        OPT_PROFILE_EXIT = 0,
        OPT_PROFILE_LIST,
        OPT_PROFILE_ADD,
        OPT_PROFILE_DEL,
    };
    enum CardInfoMenuOption {
        OPT_CARD_RETURN = 0,
        OPT_CARD_DELETE,
        OPT_CARD_TOGGLE_VISIBLE,
        OPT_CARD_COPY,
    };

    auto StartMenu(const std::string &status_msg, bool profile_exists) const -> StartMenuOption;
    void CreateProfileMenu(const std::string &status_msg, std::string &password, std::string &confirm_password) const;
    auto ProfileMenu(const std::string &status_msg) const -> ProfileMenuOption;
    auto CardListMenu(const std::vector<std::pair<uint32_t, std::string>> &cards_list) const -> int;
    auto CardInfoMenu(const std::vector<std::pair<std::string, std::string>> &card_fields, uint32_t *selected_field,
                      bool fields_visible) const -> CardInfoMenuOption;
    auto CardDeleteMenu(const std::vector<std::pair<uint32_t, std::string>> &cards_list) const -> int;

    void DisplayHashing() const;

    void PromptCardCvv(const std::string &status_msg, std::string &cvv) const;
    void PromptCardMonth(const std::string &status_msg, std::string &month) const;
    void PromptCardName(const std::string &status_msg, std::string &card_name) const;
    void PromptCardNumber(const std::string &status_msg, std::string &card_number) const;
    void PromptCardYear(const std::string &status_msg, std::string &year) const;
    void PromptLogin(std::string &password) const;
    auto PromptConfirmation(const std::string &msg) const -> bool;

  private:
    void ListCards(const std::vector<std::pair<uint32_t, std::string>> &cards_list,
                   std::unordered_map<int, uint32_t> &choice_mapping, int starting_option) const;
    auto GetSelection(int lower, int upper) const -> int;
    inline auto PromptInput() const -> std::string;
    inline auto PromptInputMasked() const -> std::string;
};

#endif // UI_HPP
