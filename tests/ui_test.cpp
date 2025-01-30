#include "ui.hpp"

#include <gtest/gtest.h>
#include <sstream>

class UITest : public ::testing::Test {
  protected:
    void SetUp() override {
        cin_buffer = cin.rdbuf();
        cout_buffer = cout.rdbuf();

        cin.rdbuf(input_stream.rdbuf());
        cout.rdbuf(output_stream.rdbuf());
    }

    void TearDown() override {
        cin.rdbuf(cin_buffer);
        cout.rdbuf(cout_buffer);
    }

    int TEST_get_selection(UI &ui, int lower, int upper) {
        return ui.get_selection(lower, upper); // Access via friend
    }

    stringstream input_stream;  // Simulates user input
    stringstream output_stream; // Captures program output

    streambuf *cin_buffer;
    streambuf *cout_buffer;
};

// start_menu
void expect_start_menu_output(string output, bool profile_exists) {
    EXPECT_NE(output.find("Welcome to WalletCache"), string::npos);
    EXPECT_NE(output.find("Select an option below:"), string::npos);
    EXPECT_NE(output.find("  (0): EXIT"), string::npos);
    EXPECT_NE(output.find("  (1): CREATE NEW PROFILE"), string::npos);

    if (profile_exists) {
        EXPECT_NE(output.find("  (2): LOGIN TO EXISTING PROFILE"), string::npos);
    } else {
        EXPECT_EQ(output.find("  (2): LOGIN TO EXISTING PROFILE"), string::npos);
    }
}

TEST_F(UITest, start_menu_InputExitWithoutProfile) {
    UI ui;
    string error_msg = "";
    bool profile_exists = false;
    input_stream << "0\n";

    UI::StartMenuOption selection = ui.start_menu(error_msg, profile_exists);

    string output = output_stream.str();
    expect_start_menu_output(output, profile_exists);
    EXPECT_EQ(selection, UI::StartMenuOption::OPT_START_EXIT);
}


TEST_F(UITest, start_menu_InputExitWithProfile) {
    UI ui;
    string error_msg = "";
    bool profile_exists = true;
    input_stream << "0\n";

    UI::StartMenuOption selection = ui.start_menu(error_msg, profile_exists);

    string output = output_stream.str();
    expect_start_menu_output(output, profile_exists);
    EXPECT_EQ(selection, UI::StartMenuOption::OPT_START_EXIT);
}

TEST_F(UITest, start_menu_InputCreateNewWithoutProfile) {
    UI ui;
    string error_msg = "";
    bool profile_exists = false;
    input_stream << "1\n";

    UI::StartMenuOption selection = ui.start_menu(error_msg, profile_exists);

    string output = output_stream.str();
    expect_start_menu_output(output, profile_exists);
    EXPECT_EQ(selection, UI::StartMenuOption::OPT_START_NEW_PROFILE);
}

TEST_F(UITest, start_menu_InputLoginWithProfile) {
    UI ui;
    string error_msg = "";
    bool profile_exists = true;
    input_stream << "2\n";

    UI::StartMenuOption selection = ui.start_menu(error_msg, profile_exists);

    string output = output_stream.str();
    expect_start_menu_output(output, profile_exists);
    EXPECT_EQ(selection, UI::StartMenuOption::OPT_START_LOGIN);
}

TEST_F(UITest, start_menu_InputLoginWithoutProfile_NotAccepted) {
    UI ui;
    string error_msg = "";
    bool profile_exists = false;
    input_stream << "2\n0\n";

    UI::StartMenuOption selection = ui.start_menu(error_msg, profile_exists);

    string output = output_stream.str();
    expect_start_menu_output(output, profile_exists);
    EXPECT_EQ(selection, UI::StartMenuOption::OPT_START_EXIT);
}

TEST_F(UITest, start_menu_InputWithErrorMessage) {
    UI ui;
    string error_msg = "ERR: Test Error!";
    bool profile_exists = true;
    input_stream << "0\n";

    UI::StartMenuOption selection = ui.start_menu(error_msg, profile_exists);

    string output = output_stream.str();
    expect_start_menu_output(output, profile_exists);
    EXPECT_NE(output.find("ERR: Test Error!"), string::npos);
    EXPECT_EQ(selection, UI::StartMenuOption::OPT_START_EXIT);
}

// create_profile_menu
TEST_F(UITest, create_profile_menu_MatchingPassword_DisplaysMessages) {
    UI ui;
    string error_msg;
    string password;
    string confirm_password;
    input_stream << "testpass\ntestpass\n"; 

    ui.create_profile_menu(error_msg, password, confirm_password);

    string output = output_stream.str();
    EXPECT_NE(output.find("Enter a master password:"), string::npos);
    EXPECT_NE(output.find("Confirm master password:"), string::npos);
    EXPECT_EQ(output.find("testpass"),
              string::npos); // Shouldn't output password
    EXPECT_EQ(password, "testpass");
    EXPECT_EQ(confirm_password, "testpass");
}

TEST_F(UITest, create_profile_menu_DifferentPassword_DisplaysMessages) {
    UI ui;
    string error_msg;
    string password;
    string confirm_password;
    input_stream << "testpass\ndiffpass\n"; 

    ui.create_profile_menu(error_msg, password, confirm_password);

    string output = output_stream.str();
    EXPECT_NE(output.find("Enter a master password:"), string::npos);
    EXPECT_NE(output.find("Confirm master password:"), string::npos);
    EXPECT_EQ(output.find("testpass"),
              string::npos); // Shouldn't output password
    EXPECT_EQ(output.find("diffpass"),
              string::npos); // Shouldn't output password
    EXPECT_EQ(password, "testpass");
    EXPECT_EQ(confirm_password, "diffpass");
}

// profile_menu
void expect_profile_menu_output(string output) {
    EXPECT_NE(output.find("Welcome to WalletCache"), string::npos);
    EXPECT_NE(output.find("Select an option below:"), string::npos);
    EXPECT_NE(output.find("  (0): EXIT"), string::npos);
    EXPECT_NE(output.find("  (1): LIST"), string::npos);
    EXPECT_NE(output.find("  (2): ADD"), string::npos);
    EXPECT_NE(output.find("  (3): DELETE"), string::npos);
}

TEST_F(UITest, profile_menu_InputExit) {
    UI ui;
    string error_msg = "";
    input_stream << "0\n";

    UI::ProfileMenuOption selection = ui.profile_menu(error_msg);

    string output = output_stream.str();
    expect_profile_menu_output(output);
    EXPECT_EQ(selection, UI::ProfileMenuOption::OPT_PROFILE_EXIT);
}

TEST_F(UITest, profile_menu_InputList) {
    UI ui;
    string error_msg = "";
    input_stream << "1\n";

    UI::ProfileMenuOption selection = ui.profile_menu(error_msg);

    string output = output_stream.str();
    expect_profile_menu_output(output);
    EXPECT_EQ(selection, UI::ProfileMenuOption::OPT_PROFILE_LIST);
}

TEST_F(UITest, profile_menu_InputAdd) {
    UI ui;
    string error_msg = "";
    input_stream << "2\n";

    UI::ProfileMenuOption selection = ui.profile_menu(error_msg);

    string output = output_stream.str();
    expect_profile_menu_output(output);
    EXPECT_EQ(selection, UI::ProfileMenuOption::OPT_PROFILE_ADD);
}

TEST_F(UITest, profile_menu_InputDelete) {
    UI ui;
    string error_msg = "";
    input_stream << "3\n";

    UI::ProfileMenuOption selection = ui.profile_menu(error_msg);

    string output = output_stream.str();
    expect_profile_menu_output(output);
    EXPECT_EQ(selection, UI::ProfileMenuOption::OPT_PROFILE_DEL);
}

TEST_F(UITest, profile_menu_InputWithErrorMessage) {
    UI ui;
    string error_msg = "ERR: Test Error!";
    input_stream << "0\n";

    UI::ProfileMenuOption selection = ui.profile_menu(error_msg);

    string output = output_stream.str();
    expect_profile_menu_output(output);
    EXPECT_NE(output.find("ERR: Test Error!"), string::npos);
    EXPECT_EQ(selection, UI::ProfileMenuOption::OPT_PROFILE_EXIT);
}

// cards_list
TEST_F(UITest, cards_list_DisplaysCards) {
    UI ui;
    const string test_cards = "Card 1\nCard 2\n";

    ui.cards_list(test_cards);

    EXPECT_NE(output_stream.str().find(test_cards), string::npos);
}

// display_hashing
TEST_F(UITest, dispay_hashing_ShowsCorrectMessage) {
    UI ui;
    ui.display_hashing();

    EXPECT_EQ(output_stream.str(), "\nHashing...\n");
}

// prompt_card_cvv
TEST_F(UITest, prompt_card_cvv_InputName) {
    UI ui;
    string error_msg = "";
    string cvv;
    input_stream << "101\n";

    ui.prompt_card_cvv(error_msg, cvv);

    EXPECT_NE(output_stream.str().find("Enter card CVV (or 0 to cancel):"), string::npos);
    EXPECT_EQ(cvv, "101");
}

TEST_F(UITest, prompt_card_cvv_InputEmptyNumber) {
    UI ui;
    string error_msg = "";
    string cvv;
    input_stream << "\n";

    ui.prompt_card_cvv(error_msg, cvv);

    EXPECT_NE(output_stream.str().find("Enter card CVV (or 0 to cancel):"), string::npos);
    EXPECT_EQ(cvv, "");
}

TEST_F(UITest, prompt_card_cvv_InputWithErrorMessage) {
    UI ui;
    string error_msg = "ERR: Test Error!";
    string cvv;
    input_stream << "211\n";

    ui.prompt_card_cvv(error_msg, cvv);

    string output = output_stream.str();
    EXPECT_NE(output_stream.str().find("Enter card CVV (or 0 to cancel):"), string::npos);
    EXPECT_NE(output.find("ERR: Test Error!"), string::npos);
    EXPECT_EQ(cvv, "211");
}

// prompt_card_month
TEST_F(UITest, prompt_card_month_InputName) {
    UI ui;
    string error_msg = "";
    string month;
    input_stream << "10\n";

    ui.prompt_card_month(error_msg, month);

    EXPECT_NE(output_stream.str().find("Enter card expiration month [ex: 10 for October] (or 0 to cancel):"),
              string::npos);
    EXPECT_EQ(month, "10");
}

TEST_F(UITest, prompt_card_month_InputEmptyNumber) {
    UI ui;
    string error_msg = "";
    string month;
    input_stream << "\n";

    ui.prompt_card_month(error_msg, month);

    EXPECT_NE(output_stream.str().find("Enter card expiration month [ex: 10 for October] (or 0 to cancel):"),
              string::npos);
    EXPECT_EQ(month, "");
}

TEST_F(UITest, prompt_card_month_InputWithErrorMessage) {
    UI ui;
    string error_msg = "ERR: Test Error!";
    string month;
    input_stream << "11\n";

    ui.prompt_card_month(error_msg, month);

    string output = output_stream.str();
    EXPECT_NE(output_stream.str().find("Enter card expiration month [ex: 10 for October] (or 0 to cancel):"),
              string::npos);
    EXPECT_NE(output.find("ERR: Test Error!"), string::npos);
    EXPECT_EQ(month, "11");
}

// prompt_card_name
TEST_F(UITest, prompt_card_name_InputName) {
    UI ui;
    string error_msg = "";
    string name;
    input_stream << "Venture One\n";

    ui.prompt_card_name(error_msg, name);

    EXPECT_NE(output_stream.str().find("Optional: enter a name for the card [only "
                                       "letters or numbers] (or 0 to cancel):"),
              string::npos);
    EXPECT_EQ(name, "Venture One");
}

TEST_F(UITest, prompt_card_name_InputEmptyName) {
    UI ui;
    string error_msg = "";
    string name;
    input_stream << "\n";

    ui.prompt_card_name(error_msg, name);

    EXPECT_NE(output_stream.str().find("Optional: enter a name for the card [only "
                                       "letters or numbers] (or 0 to cancel):"),
              string::npos);
    EXPECT_EQ(name, "");
}

TEST_F(UITest, prompt_card_name_InputWithErrorMessage) {
    UI ui;
    string error_msg = "ERR: Test Error!";
    string name;
    input_stream << "BofA Cash\n";

    ui.prompt_card_name(error_msg, name);

    string output = output_stream.str();
    EXPECT_NE(output_stream.str().find("Optional: enter a name for the card [only "
                                       "letters or numbers] (or 0 to cancel):"),
              string::npos);
    EXPECT_NE(output.find("ERR: Test Error!"), string::npos);
    EXPECT_EQ(name, "BofA Cash");
}

// prompt_card_number
TEST_F(UITest, prompt_card_number_InputNumber) {
    UI ui;
    string error_msg = "";
    string number;
    input_stream << "1111111111111111\n";

    ui.prompt_card_number(error_msg, number);

    EXPECT_NE(output_stream.str().find("Enter card number"), string::npos);
    EXPECT_EQ(number, "1111111111111111");
}

TEST_F(UITest, prompt_card_number_InputEmptyNumber) {
    UI ui;
    string error_msg = "";
    string number;
    input_stream << "\n";

    ui.prompt_card_number(error_msg, number);

    EXPECT_NE(output_stream.str().find("Enter card number"), string::npos);
    EXPECT_EQ(number, "");
}

TEST_F(UITest, prompt_card_number_InputWithErrorMessage) {
    UI ui;
    string error_msg = "ERR: Test Error!";
    string number;
    input_stream << "1111111111111111\n";

    ui.prompt_card_number(error_msg, number);

    string output = output_stream.str();
    EXPECT_NE(output.find("Enter card number"), string::npos);
    EXPECT_NE(output.find("ERR: Test Error!"), string::npos);
    EXPECT_EQ(number, "1111111111111111");
}

// prompt_card_year
TEST_F(UITest, prompt_card_year_InputYear) {
    UI ui;
    string error_msg = "";
    string year;
    input_stream << "1000\n"; 

    ui.prompt_card_year(error_msg, year);

    EXPECT_NE(output_stream.str().find("Enter card expiration year [ex: 2025] (or 0 to cancel):"), string::npos);
    EXPECT_EQ(year, "1000");
}

TEST_F(UITest, prompt_card_year_InputEmptyYear) {
    UI ui;
    string error_msg = "";
    string year;
    input_stream << "\n"; 

    ui.prompt_card_year(error_msg, year);

    EXPECT_NE(output_stream.str().find("Enter card expiration year [ex: 2025] (or 0 to cancel):"), string::npos);
    EXPECT_EQ(year, "");
}

TEST_F(UITest, prompt_card_year_InputWithErrorMessage) {
    UI ui;
    string error_msg = "ERR: Test Error!";
    string year;
    input_stream << "1000\n"; 

    ui.prompt_card_year(error_msg, year);

    string output = output_stream.str();
    EXPECT_NE(output.find("Enter card expiration year [ex: 2025] (or 0 to cancel):"), string::npos);
    EXPECT_NE(output.find("ERR: Test Error!"), string::npos);
    EXPECT_EQ(year, "1000");
}

// prompt_login
TEST_F(UITest, prompt_login_InputPassword) {
    UI ui;
    string password;
    input_stream << "s3cr3tP@ss\n"; 

    ui.prompt_login(password);

    string output = output_stream.str();
    EXPECT_NE(output.find("Enter profile password:"), string::npos);
    EXPECT_EQ(output.find("s3cr3tP@ss"),
              string::npos); // Shouldn't output password
    EXPECT_EQ(password, "s3cr3tP@ss");
}

TEST_F(UITest, prompt_login_EmptyPassword) {
    UI ui;
    string password;
    input_stream << "\n"; 

    ui.prompt_login(password);

    EXPECT_NE(output_stream.str().find("Enter profile password:"), string::npos);
    EXPECT_EQ(password, "");
}

//  prompt_confirmation
TEST_F(UITest, prompt_confirmation_Outputs) {
    UI ui;

    input_stream << "1\n";

    bool result = ui.prompt_confirmation("Do you want to proceed?");

    EXPECT_TRUE(result);

    string output = output_stream.str();
    EXPECT_TRUE(output.find("Do you want to proceed?") != string::npos); 
    EXPECT_TRUE(output.find("(  0) Cancel") != string::npos);             
    EXPECT_TRUE(output.find("(  1) Confirm") != string::npos);
}

// get_selection
TEST_F(UITest, get_selection_LowerBoundary) {
    UI ui;
    input_stream << "0\n";

    int result = TEST_get_selection(ui, 0, 1);

    EXPECT_EQ(result, 0);
}

TEST_F(UITest, get_selection_UpperBoundary) {
    UI ui;
    input_stream << "1\n";

    int result = TEST_get_selection(ui, 0, 1);

    EXPECT_EQ(result, 1);
}

TEST_F(UITest, get_selection_InvalidNumericInput) {
    UI ui;
    input_stream << "3\n1\n";

    int result = TEST_get_selection(ui, 0, 1);

    EXPECT_EQ(result, 1);
    EXPECT_TRUE(output_stream.str().find("Please input a number corresponding to available options.") !=
                string::npos); 
}

TEST_F(UITest, get_selection_InvalidNegativeNumericInput) {
    UI ui;
    input_stream << "-3\n1\n";

    int result = TEST_get_selection(ui, 0, 1);

    EXPECT_EQ(result, 1);
    EXPECT_TRUE(output_stream.str().find("Please input a number corresponding to available options.") !=
                string::npos);
}

TEST_F(UITest, get_selection_InvalidTextInput) {
    UI ui;
    input_stream << "abcabc\n1\n";

    int result = TEST_get_selection(ui, 0, 1);

    EXPECT_EQ(result, 1);
    EXPECT_TRUE(output_stream.str().find("Please input a number corresponding to available options.") !=
                string::npos); 
}

TEST_F(UITest, get_selection_EmptyInput) {
    UI ui;
    input_stream << "\n1\n";

    int result = TEST_get_selection(ui, 0, 1);

    EXPECT_EQ(result, 1);
    EXPECT_TRUE(output_stream.str().find("Please input a number corresponding to available options.") !=
                string::npos); 
}
