#include "ui.hpp"
#include "utils.hpp"

#include <iostream>
#include <unistd.h>

using namespace std;

int main() {
    UI ui = UI();
    while (true) {
        int input = ui.start_menu();
        switch (input) {
        case UI::INPUT_EXIT:
            return 0;
        case UI::INPUT_NEW_PROFILE:
            cout << "Creating new profile!\n";
            break;
        default:
            cout << "Selected: " << input << "\n";
        }

        sleep(2);
    }
}
