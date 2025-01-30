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

    StartMenuOption start_menu(string error_msg, bool profile_exists);
    void create_profile_menu(string error_msg, string &password, string &confirm_password);
    ProfileMenuOption profile_menu(string error_msg);
    void cards_list(string cards_string);

    void display_hashing();

    void prompt_card_cvv(string error_msg, string &cvv);
    void prompt_card_month(string error_msg, string &month);
    void prompt_card_name(string error_msg, string &card_name);
    void prompt_card_number(string error_msg, string &card_number);
    void prompt_card_year(string error_msg, string &year);
    void prompt_login(string &password);
    bool prompt_confirmation(string msg);

  private:
    int get_selection(int lower, int upper);
    inline string prompt_input();
    inline string prompt_input_masked();
};

#endif // UI_HPP
