#ifndef UI_HPP
#define UI_HPP

#include <string>

class UI {
  public:
    static constexpr int INPUT_EXIT = -1;
    static constexpr int INPUT_NEW_PROFILE = 0;

    int start_menu();

  private:
    inline std::string prompt_input();
};

#endif // UI_HPP
