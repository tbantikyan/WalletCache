#include "ui.hpp"
#include "utils.hpp"

#include <iostream>

using namespace std;

int UI::start_menu() {
    clear_screen();
    cout << "Welcome to WalletCache\n\n";
    cout << "Select an option below:\n";
    cout << "  -1: EXIT\n";
    cout << "   0: CREATE NEW PROFILE\n";

    string input_string;
    bool valid_input;
    do {
        input_string = this->prompt_input(); 
        valid_input = validate_input_int(input_string, -1, 3);

        if (!valid_input) {
            cout << "Please input a number corresponding to available options.\n";
        }
    } while (!valid_input);
    
    return stoi(input_string);
}

string UI::prompt_input() {
    cout << "> ";
    string input;
    cin >> input;
    return input; 
}
