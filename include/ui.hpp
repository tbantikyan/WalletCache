#ifndef UI_HPP
#define UI_HPP

#include <string>

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

    auto StartMenu(const std::string &status_msg, bool profile_exists) -> StartMenuOption;
    void CreateProfileMenu(const std::string &status_msg, std::string &password, std::string &confirm_password);
    auto ProfileMenu(const std::string &status_msg) -> ProfileMenuOption;
    void CardsList(const std::string &cards_string);

    void DisplayHashing();

    void PromptCardCvv(const std::string &status_msg, std::string &cvv);
    void PromptCardMonth(const std::string &status_msg, std::string &month);
    void PromptCardName(const std::string &status_msg, std::string &card_name);
    void PromptCardNumber(const std::string &status_msg, std::string &card_number);
    void PromptCardYear(const std::string &status_msg, std::string &year);
    void PromptLogin(std::string &password);
    auto PromptConfirmation(const std::string &msg) -> bool;

  private:
    auto GetSelection(int lower, int upper) -> int;
    inline auto PromptInput() -> std::string;
    inline auto PromptInputMasked() -> std::string;
};

#endif // UI_HPP
