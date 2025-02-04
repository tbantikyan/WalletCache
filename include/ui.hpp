#ifndef UI_HPP
#define UI_HPP

#include <string>
using namespace std;

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

    StartMenuOption StartMenu(string error_msg, bool profile_exists);
    void CreateProfileMenu(string error_msg, string &password, string &confirm_password);
    ProfileMenuOption ProfileMenu(string error_msg);
    void CardsList(string cards_string);

    void DisplayHashing();

    void PromptCardCvv(string error_msg, string &cvv);
    void PromptCardMonth(string error_msg, string &month);
    void PromptCardName(string error_msg, string &card_name);
    void PromptCardNumber(string error_msg, string &card_number);
    void PromptCardYear(string error_msg, string &year);
    void PromptLogin(string &password);
    bool PromptConfirmation(string msg);

  private:
    int GetSelection(int lower, int upper);
    inline string PromptInput();
    inline string PromptInputMasked();
};

#endif // UI_HPP
