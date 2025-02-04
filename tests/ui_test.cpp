#include "ui.hpp"

#include <gtest/gtest.h>
#include <sstream>

class UITest : public ::testing::Test {
  protected:
    void SetUp() override {
        cin_buffer_ = std::cin.rdbuf();
        cout_buffer_ = std::cout.rdbuf();

        std::cin.rdbuf(input_stream_.rdbuf());
        std::cout.rdbuf(output_stream_.rdbuf());
    }

    void TearDown() override {
        std::cin.rdbuf(cin_buffer_);
        std::cout.rdbuf(cout_buffer_);
    }

    auto TestGetSelection(UI &ui, int lower, int upper) -> int {
        return ui.GetSelection(lower, upper); // Access via friend
    }

    std::stringstream input_stream_;  // Simulates user input
    std::stringstream output_stream_; // Captures program output

    std::streambuf *cin_buffer_;
    std::streambuf *cout_buffer_;
};

// StartMenu
void ExpectStartMenuOutput(const std::string &output, bool profile_exists) {
    EXPECT_NE(output.find("Welcome to WalletCache"), std::string::npos);
    EXPECT_NE(output.find("Select an option below:"), std::string::npos);
    EXPECT_NE(output.find("  (0): EXIT"), std::string::npos);
    EXPECT_NE(output.find("  (1): CREATE NEW PROFILE"), std::string::npos);

    if (profile_exists) {
        EXPECT_NE(output.find("  (2): LOGIN TO EXISTING PROFILE"), std::string::npos);
    } else {
        EXPECT_EQ(output.find("  (2): LOGIN TO EXISTING PROFILE"), std::string::npos);
    }
}

TEST_F(UITest, StartMenu_InputExitWithoutProfile) {
    UI ui;
    std::string error_msg;
    bool profile_exists = false;
    input_stream_ << "0\n";

    UI::StartMenuOption selection = ui.StartMenu(error_msg, profile_exists);

    std::string output = output_stream_.str();
    ExpectStartMenuOutput(output, profile_exists);
    EXPECT_EQ(selection, UI::StartMenuOption::OPT_START_EXIT);
}

TEST_F(UITest, StartMenu_InputExitWithProfile) {
    UI ui;
    std::string error_msg;
    bool profile_exists = true;
    input_stream_ << "0\n";

    UI::StartMenuOption selection = ui.StartMenu(error_msg, profile_exists);

    std::string output = output_stream_.str();
    ExpectStartMenuOutput(output, profile_exists);
    EXPECT_EQ(selection, UI::StartMenuOption::OPT_START_EXIT);
}

TEST_F(UITest, StartMenu_InputCreateNewWithoutProfile) {
    UI ui;
    std::string error_msg;
    bool profile_exists = false;
    input_stream_ << "1\n";

    UI::StartMenuOption selection = ui.StartMenu(error_msg, profile_exists);

    std::string output = output_stream_.str();
    ExpectStartMenuOutput(output, profile_exists);
    EXPECT_EQ(selection, UI::StartMenuOption::OPT_START_NEW_PROFILE);
}

TEST_F(UITest, StartMenu_InputLoginWithProfile) {
    UI ui;
    std::string error_msg;
    bool profile_exists = true;
    input_stream_ << "2\n";

    UI::StartMenuOption selection = ui.StartMenu(error_msg, profile_exists);

    std::string output = output_stream_.str();
    ExpectStartMenuOutput(output, profile_exists);
    EXPECT_EQ(selection, UI::StartMenuOption::OPT_START_LOGIN);
}

TEST_F(UITest, StartMenu_InputLoginWithoutProfile_NotAccepted) {
    UI ui;
    std::string error_msg;
    bool profile_exists = false;
    input_stream_ << "2\n0\n";

    UI::StartMenuOption selection = ui.StartMenu(error_msg, profile_exists);

    std::string output = output_stream_.str();
    ExpectStartMenuOutput(output, profile_exists);
    EXPECT_EQ(selection, UI::StartMenuOption::OPT_START_EXIT);
}

TEST_F(UITest, StartMenu_InputWithErrorMessage) {
    UI ui;
    std::string error_msg = "ERR: Test Error!";
    bool profile_exists = true;
    input_stream_ << "0\n";

    UI::StartMenuOption selection = ui.StartMenu(error_msg, profile_exists);

    std::string output = output_stream_.str();
    ExpectStartMenuOutput(output, profile_exists);
    EXPECT_NE(output.find("ERR: Test Error!"), std::string::npos);
    EXPECT_EQ(selection, UI::StartMenuOption::OPT_START_EXIT);
}

// CreateProfileMenu
TEST_F(UITest, CreateProfileMenu_MatchingPassword_DisplaysMessages) {
    UI ui;
    std::string error_msg;
    std::string password;
    std::string confirm_password;
    input_stream_ << "testpass\ntestpass\n";

    ui.CreateProfileMenu(error_msg, password, confirm_password);

    std::string output = output_stream_.str();
    EXPECT_NE(output.find("Enter a master password:"), std::string::npos);
    EXPECT_NE(output.find("Confirm master password:"), std::string::npos);
    EXPECT_EQ(output.find("testpass"),
              std::string::npos); // Shouldn't output password
    EXPECT_EQ(password, "testpass");
    EXPECT_EQ(confirm_password, "testpass");
}

TEST_F(UITest, CreateProfileMenu_DifferentPassword_DisplaysMessages) {
    UI ui;
    std::string error_msg;
    std::string password;
    std::string confirm_password;
    input_stream_ << "testpass\ndiffpass\n";

    ui.CreateProfileMenu(error_msg, password, confirm_password);

    std::string output = output_stream_.str();
    EXPECT_NE(output.find("Enter a master password:"), std::string::npos);
    EXPECT_NE(output.find("Confirm master password:"), std::string::npos);
    EXPECT_EQ(output.find("testpass"),
              std::string::npos); // Shouldn't output password
    EXPECT_EQ(output.find("diffpass"),
              std::string::npos); // Shouldn't output password
    EXPECT_EQ(password, "testpass");
    EXPECT_EQ(confirm_password, "diffpass");
}

// ProfileMenu
void ExpectProfileMenuOutput(const std::string &output) {
    EXPECT_NE(output.find("Welcome to WalletCache"), std::string::npos);
    EXPECT_NE(output.find("Select an option below:"), std::string::npos);
    EXPECT_NE(output.find("  (0): EXIT"), std::string::npos);
    EXPECT_NE(output.find("  (1): LIST"), std::string::npos);
    EXPECT_NE(output.find("  (2): ADD"), std::string::npos);
    EXPECT_NE(output.find("  (3): DELETE"), std::string::npos);
}

TEST_F(UITest, ProfileMenu_InputExit) {
    UI ui;
    std::string error_msg;
    input_stream_ << "0\n";

    UI::ProfileMenuOption selection = ui.ProfileMenu(error_msg);

    std::string output = output_stream_.str();
    ExpectProfileMenuOutput(output);
    EXPECT_EQ(selection, UI::ProfileMenuOption::OPT_PROFILE_EXIT);
}

TEST_F(UITest, ProfileMenu_InputList) {
    UI ui;
    std::string error_msg;
    input_stream_ << "1\n";

    UI::ProfileMenuOption selection = ui.ProfileMenu(error_msg);

    std::string output = output_stream_.str();
    ExpectProfileMenuOutput(output);
    EXPECT_EQ(selection, UI::ProfileMenuOption::OPT_PROFILE_LIST);
}

TEST_F(UITest, ProfileMenu_InputAdd) {
    UI ui;
    std::string error_msg;
    input_stream_ << "2\n";

    UI::ProfileMenuOption selection = ui.ProfileMenu(error_msg);

    std::string output = output_stream_.str();
    ExpectProfileMenuOutput(output);
    EXPECT_EQ(selection, UI::ProfileMenuOption::OPT_PROFILE_ADD);
}

TEST_F(UITest, ProfileMenu_InputDelete) {
    UI ui;
    std::string error_msg;
    input_stream_ << "3\n";

    UI::ProfileMenuOption selection = ui.ProfileMenu(error_msg);

    std::string output = output_stream_.str();
    ExpectProfileMenuOutput(output);
    EXPECT_EQ(selection, UI::ProfileMenuOption::OPT_PROFILE_DEL);
}

TEST_F(UITest, ProfileMenu_InputWithErrorMessage) {
    UI ui;
    std::string error_msg = "ERR: Test Error!";
    input_stream_ << "0\n";

    UI::ProfileMenuOption selection = ui.ProfileMenu(error_msg);

    std::string output = output_stream_.str();
    ExpectProfileMenuOutput(output);
    EXPECT_NE(output.find("ERR: Test Error!"), std::string::npos);
    EXPECT_EQ(selection, UI::ProfileMenuOption::OPT_PROFILE_EXIT);
}

// CardsList
TEST_F(UITest, CardsList_DisplaysCards) {
    UI ui;
    const std::string test_cards = "Card 1\nCard 2\n";

    ui.CardsList(test_cards);

    EXPECT_NE(output_stream_.str().find(test_cards), std::string::npos);
}

// DisplayHashing
TEST_F(UITest, DisplayHashing_ShowsCorrectMessage) {
    UI ui;
    ui.DisplayHashing();

    EXPECT_EQ(output_stream_.str(), "\nHashing...\n");
}

// PromptCardCvv
TEST_F(UITest, PromptCardCvv_InputName) {
    UI ui;
    std::string error_msg;
    std::string cvv;
    input_stream_ << "101\n";

    ui.PromptCardCvv(error_msg, cvv);

    EXPECT_NE(output_stream_.str().find("Enter card CVV (or 0 to cancel):"), std::string::npos);
    EXPECT_EQ(cvv, "101");
}

TEST_F(UITest, PromptCardCvv_InputEmptyNumber) {
    UI ui;
    std::string error_msg;
    std::string cvv;
    input_stream_ << "\n";

    ui.PromptCardCvv(error_msg, cvv);

    EXPECT_NE(output_stream_.str().find("Enter card CVV (or 0 to cancel):"), std::string::npos);
    EXPECT_EQ(cvv, "");
}

TEST_F(UITest, PromptCardCvv_InputWithErrorMessage) {
    UI ui;
    std::string error_msg = "ERR: Test Error!";
    std::string cvv;
    input_stream_ << "211\n";

    ui.PromptCardCvv(error_msg, cvv);

    std::string output = output_stream_.str();
    EXPECT_NE(output_stream_.str().find("Enter card CVV (or 0 to cancel):"), std::string::npos);
    EXPECT_NE(output.find("ERR: Test Error!"), std::string::npos);
    EXPECT_EQ(cvv, "211");
}

// PromptCardMonth
TEST_F(UITest, PromptCardMonth_InputName) {
    UI ui;
    std::string error_msg;
    std::string month;
    input_stream_ << "10\n";

    ui.PromptCardMonth(error_msg, month);

    EXPECT_NE(output_stream_.str().find("Enter card expiration month [ex: 10 for October] (or 0 to cancel):"),
              std::string::npos);
    EXPECT_EQ(month, "10");
}

TEST_F(UITest, PromptCardMonth_InputEmptyNumber) {
    UI ui;
    std::string error_msg;
    std::string month;
    input_stream_ << "\n";

    ui.PromptCardMonth(error_msg, month);

    EXPECT_NE(output_stream_.str().find("Enter card expiration month [ex: 10 for October] (or 0 to cancel):"),
              std::string::npos);
    EXPECT_EQ(month, "");
}

TEST_F(UITest, PromptCardMonth_InputWithErrorMessage) {
    UI ui;
    std::string error_msg = "ERR: Test Error!";
    std::string month;
    input_stream_ << "11\n";

    ui.PromptCardMonth(error_msg, month);

    std::string output = output_stream_.str();
    EXPECT_NE(output_stream_.str().find("Enter card expiration month [ex: 10 for October] (or 0 to cancel):"),
              std::string::npos);
    EXPECT_NE(output.find("ERR: Test Error!"), std::string::npos);
    EXPECT_EQ(month, "11");
}

// PromptCardName
TEST_F(UITest, PromptCardName_InputName) {
    UI ui;
    std::string error_msg;
    std::string name;
    input_stream_ << "Venture One\n";

    ui.PromptCardName(error_msg, name);

    EXPECT_NE(output_stream_.str().find("Optional: enter a name for the card [only "
                                        "letters or numbers] (or 0 to cancel):"),
              std::string::npos);
    EXPECT_EQ(name, "Venture One");
}

TEST_F(UITest, PromptCardName_InputEmptyName) {
    UI ui;
    std::string error_msg;
    std::string name;
    input_stream_ << "\n";

    ui.PromptCardName(error_msg, name);

    EXPECT_NE(output_stream_.str().find("Optional: enter a name for the card [only "
                                        "letters or numbers] (or 0 to cancel):"),
              std::string::npos);
    EXPECT_EQ(name, "");
}

TEST_F(UITest, PromptCardName_InputWithErrorMessage) {
    UI ui;
    std::string error_msg = "ERR: Test Error!";
    std::string name;
    input_stream_ << "BofA Cash\n";

    ui.PromptCardName(error_msg, name);

    std::string output = output_stream_.str();
    EXPECT_NE(output_stream_.str().find("Optional: enter a name for the card [only "
                                        "letters or numbers] (or 0 to cancel):"),
              std::string::npos);
    EXPECT_NE(output.find("ERR: Test Error!"), std::string::npos);
    EXPECT_EQ(name, "BofA Cash");
}

// PromptCardNumber
TEST_F(UITest, PromptCardNumber_InputNumber) {
    UI ui;
    std::string error_msg;
    std::string number;
    input_stream_ << "1111111111111111\n";

    ui.PromptCardNumber(error_msg, number);

    EXPECT_NE(output_stream_.str().find("Enter card number"), std::string::npos);
    EXPECT_EQ(number, "1111111111111111");
}

TEST_F(UITest, PromptCardNumber_InputEmptyNumber) {
    UI ui;
    std::string error_msg;
    std::string number;
    input_stream_ << "\n";

    ui.PromptCardNumber(error_msg, number);

    EXPECT_NE(output_stream_.str().find("Enter card number"), std::string::npos);
    EXPECT_EQ(number, "");
}

TEST_F(UITest, PromptCardNumber_InputWithErrorMessage) {
    UI ui;
    std::string error_msg = "ERR: Test Error!";
    std::string number;
    input_stream_ << "1111111111111111\n";

    ui.PromptCardNumber(error_msg, number);

    std::string output = output_stream_.str();
    EXPECT_NE(output.find("Enter card number"), std::string::npos);
    EXPECT_NE(output.find("ERR: Test Error!"), std::string::npos);
    EXPECT_EQ(number, "1111111111111111");
}

// PromptCardYear
TEST_F(UITest, PromptCardYear_InputYear) {
    UI ui;
    std::string error_msg;
    std::string year;
    input_stream_ << "1000\n";

    ui.PromptCardYear(error_msg, year);

    EXPECT_NE(output_stream_.str().find("Enter card expiration year [ex: 2025] (or 0 to cancel):"), std::string::npos);
    EXPECT_EQ(year, "1000");
}

TEST_F(UITest, PromptCardYear_InputEmptyYear) {
    UI ui;
    std::string error_msg;
    std::string year;
    input_stream_ << "\n";

    ui.PromptCardYear(error_msg, year);

    EXPECT_NE(output_stream_.str().find("Enter card expiration year [ex: 2025] (or 0 to cancel):"), std::string::npos);
    EXPECT_EQ(year, "");
}

TEST_F(UITest, PromptCardYear_InputWithErrorMessage) {
    UI ui;
    std::string error_msg = "ERR: Test Error!";
    std::string year;
    input_stream_ << "1000\n";

    ui.PromptCardYear(error_msg, year);

    std::string output = output_stream_.str();
    EXPECT_NE(output.find("Enter card expiration year [ex: 2025] (or 0 to cancel):"), std::string::npos);
    EXPECT_NE(output.find("ERR: Test Error!"), std::string::npos);
    EXPECT_EQ(year, "1000");
}

// PromptLogin
TEST_F(UITest, PromptLogin_InputPassword) {
    UI ui;
    std::string password;
    input_stream_ << "s3cr3tP@ss\n";

    ui.PromptLogin(password);

    std::string output = output_stream_.str();
    EXPECT_NE(output.find("Enter profile password:"), std::string::npos);
    EXPECT_EQ(output.find("s3cr3tP@ss"),
              std::string::npos); // Shouldn't output password
    EXPECT_EQ(password, "s3cr3tP@ss");
}

TEST_F(UITest, PromptLogin_EmptyPassword) {
    UI ui;
    std::string password;
    input_stream_ << "\n";

    ui.PromptLogin(password);

    EXPECT_NE(output_stream_.str().find("Enter profile password:"), std::string::npos);
    EXPECT_EQ(password, "");
}

//  PromptConfirmation
TEST_F(UITest, PromptConfirmation_Outputs) {
    UI ui;

    input_stream_ << "1\n";

    bool result = ui.PromptConfirmation("Do you want to proceed?");

    EXPECT_TRUE(result);

    std::string output = output_stream_.str();
    EXPECT_TRUE(output.find("Do you want to proceed?") != std::string::npos);
    EXPECT_TRUE(output.find("(  0) Cancel") != std::string::npos);
    EXPECT_TRUE(output.find("(  1) Confirm") != std::string::npos);
}

// GetSelection
TEST_F(UITest, GetSelection_LowerBoundary) {
    UI ui;
    input_stream_ << "0\n";

    int result = TestGetSelection(ui, 0, 1);

    EXPECT_EQ(result, 0);
}

TEST_F(UITest, GetSelection_UpperBoundary) {
    UI ui;
    input_stream_ << "1\n";

    int result = TestGetSelection(ui, 0, 1);

    EXPECT_EQ(result, 1);
}

TEST_F(UITest, GetSelection_InvalidNumericInput) {
    UI ui;
    input_stream_ << "3\n1\n";

    int result = TestGetSelection(ui, 0, 1);

    EXPECT_EQ(result, 1);
    EXPECT_TRUE(output_stream_.str().find("Please input a number corresponding to available options.") !=
                std::string::npos);
}

TEST_F(UITest, GetSelection_InvalidNegativeNumericInput) {
    UI ui;
    input_stream_ << "-3\n1\n";

    int result = TestGetSelection(ui, 0, 1);

    EXPECT_EQ(result, 1);
    EXPECT_TRUE(output_stream_.str().find("Please input a number corresponding to available options.") !=
                std::string::npos);
}

TEST_F(UITest, GetSelection_InvalidTextInput) {
    UI ui;
    input_stream_ << "abcabc\n1\n";

    int result = TestGetSelection(ui, 0, 1);

    EXPECT_EQ(result, 1);
    EXPECT_TRUE(output_stream_.str().find("Please input a number corresponding to available options.") !=
                std::string::npos);
}

TEST_F(UITest, GetSelection_EmptyInput) {
    UI ui;
    input_stream_ << "\n1\n";

    int result = TestGetSelection(ui, 0, 1);

    EXPECT_EQ(result, 1);
    EXPECT_TRUE(output_stream_.str().find("Please input a number corresponding to available options.") !=
                std::string::npos);
}
