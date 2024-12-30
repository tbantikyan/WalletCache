#include "include/util.hpp"
#include <iostream>

using namespace std;

int main() {
  string user_input;
  string info_string = "Type \"-1\" to quit.\n";

  clear_screen();
  cout << info_string;
  while (user_input != "-1") {
    cin >> user_input;
    clear_screen();
    cout << info_string;
    cout << "You said: " << user_input << "\n";
  }

  return 0;
}
