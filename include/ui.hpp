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

    int start_menu(string error_msg, bool profile_exists);
    int create_profile_menu(unsigned char *password);

  private:
    int get_selection(int lower, int upper);
    bool prompt_confirmation(string msg);
    inline string prompt_input();
    inline string prompt_input_masked();
};

#endif // UI_HPP
