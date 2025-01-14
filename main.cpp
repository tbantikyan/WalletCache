#include "store.hpp"
#include "ui.hpp"
#include "utils.hpp"
#include "verification.hpp"

#include <iostream>
#include <unistd.h>

using namespace std;

int main() {
    UI ui = UI();
    Store store = Store();
    unsigned char password[MAX_PASSWORD_LENGTH + 1];

    bool profile_exists = store.data_exists();
    string status_msg;
    while (true) {
        int input = ui.start_menu(status_msg, profile_exists);
        switch (input) {
        case UI::OPTION_EXIT:
            return 0;
        case UI::OPTION_NEW_PROFILE:
            if (ui.create_profile_menu(password) != 0) {
                status_msg = "ERR: Failed to create new profile\n";
                break;
            }
            if (store.init_new_store(password) != 0) {
                status_msg =
                    "ERR: Failed to initialize store for new profile\n";
                break;
            }

            status_msg = "Successfully created new profile!\n";
            break;
        default:
            cout << "Selected: " << input << "\n";
        }
    }
}
