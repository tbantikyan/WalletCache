#ifndef UI_HPP
#define UI_HPP

#include <string>
using namespace std;

class UI {
  public:
    enum MenuOption {
        OPTION_EXIT = 0,
        OPTION_NEW_PROFILE,
        OPTION_LOGIN,
    };

    MenuOption start_menu(string error_msg, bool profile_exists);
    void create_profile_menu(string error_msg, string &password, string &confirm_password);
    int prompt_login(unsigned char *password);
    bool prompt_confirmation(string msg);

  private:
    int get_selection(int lower, int upper);
    inline string prompt_input();
    inline string prompt_input_masked();
};

#endif // UI_HPP
