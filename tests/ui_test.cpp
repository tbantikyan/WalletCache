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
    EXPECT_NE(output.find(UIStrings::WELCOME), std::string::npos);
    EXPECT_NE(output.find(UIStrings::SELECT_OPT), std::string::npos);
    EXPECT_NE(output.find(UIStrings::START_MENU_EXIT), std::string::npos);
    EXPECT_NE(output.find(UIStrings::START_MENU_CREATE_PROFILE), std::string::npos);

    if (profile_exists) {
        EXPECT_NE(output.find(UIStrings::START_MENU_LOGIN_PROFILE), std::string::npos);
    } else {
        EXPECT_EQ(output.find(UIStrings::START_MENU_LOGIN_PROFILE), std::string::npos);
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
    EXPECT_NE(output.find(UIStrings::CREATE_PROFILE_MENU_PASSWORD), std::string::npos);
    EXPECT_NE(output.find(UIStrings::CREATE_PROFILE_MENU_CONFIRM_PASSWORD), std::string::npos);
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
    EXPECT_NE(output.find(UIStrings::CREATE_PROFILE_MENU_PASSWORD), std::string::npos);
    EXPECT_NE(output.find(UIStrings::CREATE_PROFILE_MENU_CONFIRM_PASSWORD), std::string::npos);
    EXPECT_EQ(output.find("testpass"),
              std::string::npos); // Shouldn't output password
    EXPECT_EQ(output.find("diffpass"),
              std::string::npos); // Shouldn't output password
    EXPECT_EQ(password, "testpass");
    EXPECT_EQ(confirm_password, "diffpass");
}

// ProfileMenu
void ExpectProfileMenuOutput(const std::string &output) {
    EXPECT_NE(output.find(UIStrings::WELCOME), std::string::npos);
    EXPECT_NE(output.find(UIStrings::SELECT_OPT), std::string::npos);
    EXPECT_NE(output.find(UIStrings::PROFILE_MENU_EXIT), std::string::npos);
    EXPECT_NE(output.find(UIStrings::PROFILE_MENU_LIST), std::string::npos);
    EXPECT_NE(output.find(UIStrings::PROFILE_MENU_ADD), std::string::npos);
    EXPECT_NE(output.find(UIStrings::PROFILE_MENU_DELETE), std::string::npos);
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

// ListCardsMenu
TEST_F(UITest, ListCardsMenu_InputReturn) {
    UI ui;
    const std::vector<std::pair<uint32_t, std::string>> test_list = {
        std::make_pair(0, "Card 1"),
        std::make_pair(1, "Card 2"),
    };
    input_stream_ << "0\n";

    int selection = ui.ListCardsMenu(test_list);

    EXPECT_NE(output_stream_.str().find(UIStrings::LIST_CARDS_RETURN), std::string::npos);
    EXPECT_NE(output_stream_.str().find("Card 1"), std::string::npos);
    EXPECT_NE(output_stream_.str().find("Card 2"), std::string::npos);
    EXPECT_EQ(selection, -1);
}

TEST_F(UITest, ListCardsMenu_TwoCards_InputFirstCard) {
    UI ui;
    const std::vector<std::pair<uint32_t, std::string>> test_list = {
        std::make_pair(0, "Card 1"),
        std::make_pair(1, "Card 2"),
    };
    input_stream_ << "1\n";

    int selection = ui.ListCardsMenu(test_list);

    EXPECT_NE(output_stream_.str().find(UIStrings::LIST_CARDS_RETURN), std::string::npos);
    EXPECT_NE(output_stream_.str().find("Card 1"), std::string::npos);
    EXPECT_NE(output_stream_.str().find("Card 2"), std::string::npos);
    EXPECT_EQ(selection, 0);
}

TEST_F(UITest, ListCardsMenu_TwoCards_InputSecondCard) {
    UI ui;
    const std::vector<std::pair<uint32_t, std::string>> test_list = {
        std::make_pair(0, "Card 1"),
        std::make_pair(1, "Card 2"),
    };
    input_stream_ << "2\n";

    int selection = ui.ListCardsMenu(test_list);

    EXPECT_NE(output_stream_.str().find(UIStrings::LIST_CARDS_RETURN), std::string::npos);
    EXPECT_NE(output_stream_.str().find("Card 1"), std::string::npos);
    EXPECT_NE(output_stream_.str().find("Card 2"), std::string::npos);
    EXPECT_EQ(selection, 1);
}

// CardInfoMenu
void CardInfoMenuExpectOptions(const std::string &output) {
    EXPECT_NE(output.find(UIStrings::CARD_INFO_RETURN), std::string::npos);
    EXPECT_NE(output.find(UIStrings::CARD_INFO_DELETE), std::string::npos);
    EXPECT_NE(output.find(UIStrings::CARD_INFO_TOGGLE_VISIBILITY), std::string::npos);

    EXPECT_NE(output.find(UIStrings::CARD_NAME_LABEL), std::string::npos);
    EXPECT_NE(output.find(UIStrings::CARD_NUMBER_LABEL), std::string::npos);
    EXPECT_NE(output.find(UIStrings::CARD_CVV_LABEL), std::string::npos);
    EXPECT_NE(output.find(UIStrings::CARD_MONTH_LABEL), std::string::npos);
    EXPECT_NE(output.find(UIStrings::CARD_YEAR_LABEL), std::string::npos);
}

void CardInfoMenuExpectFieldsHidden(const std::string &output,
                                    const std::vector<std::pair<std::string, std::string>> &fields) {
    EXPECT_NE(output.find(fields[0].second), std::string::npos);
    EXPECT_EQ(output.find(fields[1].second), std::string::npos);
    EXPECT_EQ(output.find(fields[2].second), std::string::npos);
    EXPECT_EQ(output.find(fields[3].second), std::string::npos);
    EXPECT_EQ(output.find(fields[4].second), std::string::npos);

    EXPECT_NE(output.find(UIStrings::HIDDEN_FIELD), std::string::npos);
}
void CardInfoMenuExpectFieldsVisible(const std::string &output,
                                    const std::vector<std::pair<std::string, std::string>> &fields) {
    EXPECT_NE(output.find(fields[0].second), std::string::npos);
    EXPECT_NE(output.find(fields[1].second), std::string::npos);
    EXPECT_NE(output.find(fields[2].second), std::string::npos);
    EXPECT_NE(output.find(fields[3].second), std::string::npos);
    EXPECT_NE(output.find(fields[4].second), std::string::npos);

    EXPECT_EQ(output.find(UIStrings::HIDDEN_FIELD), std::string::npos);
}

TEST_F(UITest, CardInfoMenu_FieldsHiddenAndSelectReturn_ReturnsRETURN) {
    UI ui;
    const std::vector<std::pair<std::string, std::string>> fields = {
        {UIStrings::CARD_NAME_LABEL, "Visa 1111"}, {UIStrings::CARD_NUMBER_LABEL, "4111111111111111"},
        {UIStrings::CARD_CVV_LABEL, "761"},        {UIStrings::CARD_MONTH_LABEL, "10"},
        {UIStrings::CARD_YEAR_LABEL, "2024"},
    };
    input_stream_ << "0\n";
    uint32_t selected_field;
    UI::CardInfoMenuOption selected_option = ui.CardInfoMenu(fields, &selected_field, /* fields_visible */ false);

    std::string output = output_stream_.str();
    CardInfoMenuExpectOptions(output);
    CardInfoMenuExpectFieldsHidden(output, fields);
    EXPECT_EQ(selected_option, UI::CardInfoMenuOption::OPT_CARD_RETURN);
}

TEST_F(UITest, CardInfoMenu_FieldsVisibleAndSelectReturn_ReturnsReturn) {
    UI ui;
    const std::vector<std::pair<std::string, std::string>> fields = {
        {UIStrings::CARD_NAME_LABEL, "Visa 1111"}, {UIStrings::CARD_NUMBER_LABEL, "4111111111111111"},
        {UIStrings::CARD_CVV_LABEL, "761"},        {UIStrings::CARD_MONTH_LABEL, "10"},
        {UIStrings::CARD_YEAR_LABEL, "2024"},
    };
    input_stream_ << "0\n";
    uint32_t selected_field;
    UI::CardInfoMenuOption selected_option = ui.CardInfoMenu(fields, &selected_field, /* fields_visible */ true);

    std::string output = output_stream_.str();
    CardInfoMenuExpectOptions(output);
    CardInfoMenuExpectFieldsVisible(output, fields);
    EXPECT_EQ(selected_option, UI::CardInfoMenuOption::OPT_CARD_RETURN);
}

TEST_F(UITest, CardInfoMenu_FieldsVisibleAndSelectDelete_ReturnsDelete) {
    UI ui;
    const std::vector<std::pair<std::string, std::string>> fields = {
        {UIStrings::CARD_NAME_LABEL, "Visa 1111"}, {UIStrings::CARD_NUMBER_LABEL, "4111111111111111"},
        {UIStrings::CARD_CVV_LABEL, "761"},        {UIStrings::CARD_MONTH_LABEL, "10"},
        {UIStrings::CARD_YEAR_LABEL, "2024"},
    };
    input_stream_ << "1\n";
    uint32_t selected_field;
    UI::CardInfoMenuOption selected_option = ui.CardInfoMenu(fields, &selected_field, /* fields_visible */ true);

    std::string output = output_stream_.str();
    CardInfoMenuExpectOptions(output);
    CardInfoMenuExpectFieldsVisible(output, fields);
    EXPECT_EQ(selected_option, UI::CardInfoMenuOption::OPT_CARD_DELETE);
}

TEST_F(UITest, CardInfoMenu_FieldsHiddenAndSelectToggleVisbility_ReturnsToggleVisiblity) {
    UI ui;
    const std::vector<std::pair<std::string, std::string>> fields = {
        {UIStrings::CARD_NAME_LABEL, "Visa 1111"}, {UIStrings::CARD_NUMBER_LABEL, "4111111111111111"},
        {UIStrings::CARD_CVV_LABEL, "761"},        {UIStrings::CARD_MONTH_LABEL, "10"},
        {UIStrings::CARD_YEAR_LABEL, "2024"},
    };
    input_stream_ << "2\n";
    uint32_t selected_field;
    UI::CardInfoMenuOption selected_option = ui.CardInfoMenu(fields, &selected_field, /* fields_visible */ false);

    std::string output = output_stream_.str();
    CardInfoMenuExpectOptions(output);
    CardInfoMenuExpectFieldsHidden(output, fields);
    EXPECT_EQ(selected_option, UI::CardInfoMenuOption::OPT_CARD_TOGGLE_VISIBLE);
}

TEST_F(UITest, CardInfoMenu_FieldsHiddenAndSelectFieldThree_ReturnsCopyAndFieldZero) {
    UI ui;
    const std::vector<std::pair<std::string, std::string>> fields = {
        {UIStrings::CARD_NAME_LABEL, "Visa 1111"}, {UIStrings::CARD_NUMBER_LABEL, "4111111111111111"},
        {UIStrings::CARD_CVV_LABEL, "761"},        {UIStrings::CARD_MONTH_LABEL, "10"},
        {UIStrings::CARD_YEAR_LABEL, "2024"},
    };
    input_stream_ << "3\n";
    uint32_t selected_field;
    UI::CardInfoMenuOption selected_option = ui.CardInfoMenu(fields, &selected_field, /* fields_visible */ false);

    std::string output = output_stream_.str();
    CardInfoMenuExpectOptions(output);
    CardInfoMenuExpectFieldsHidden(output, fields);
    EXPECT_EQ(selected_option, UI::CardInfoMenuOption::OPT_CARD_COPY);
    EXPECT_EQ(selected_field, 0); 
}

TEST_F(UITest, CardInfoMenu_FieldsVisibleAndSelectFieldSeven_ReturnsCopyAndFieldFour) {
    UI ui;
    const std::vector<std::pair<std::string, std::string>> fields = {
        {UIStrings::CARD_NAME_LABEL, "Visa 1111"}, {UIStrings::CARD_NUMBER_LABEL, "4111111111111111"},
        {UIStrings::CARD_CVV_LABEL, "761"},        {UIStrings::CARD_MONTH_LABEL, "10"},
        {UIStrings::CARD_YEAR_LABEL, "2024"},
    };
    input_stream_ << "7\n";
    uint32_t selected_field;
    UI::CardInfoMenuOption selected_option = ui.CardInfoMenu(fields, &selected_field, /* fields_visible */ true);

    std::string output = output_stream_.str();
    CardInfoMenuExpectOptions(output);
    CardInfoMenuExpectFieldsVisible(output, fields);
    EXPECT_EQ(selected_option, UI::CardInfoMenuOption::OPT_CARD_COPY);
    EXPECT_EQ(selected_field, 4); 
}

// DisplayHashing
TEST_F(UITest, DisplayHashing_ShowsCorrectMessage) {
    UI ui;
    ui.DisplayHashing();

    EXPECT_EQ(output_stream_.str(), UIStrings::HASHING);
}

// PromptCardCvv
TEST_F(UITest, PromptCardCvv_InputName) {
    UI ui;
    std::string error_msg;
    std::string cvv;
    input_stream_ << "101\n";

    ui.PromptCardCvv(error_msg, cvv);

    EXPECT_NE(output_stream_.str().find(UIStrings::CARD_CVV_PROMPT), std::string::npos);
    EXPECT_EQ(cvv, "101");
}

TEST_F(UITest, PromptCardCvv_InputEmptyNumber) {
    UI ui;
    std::string error_msg;
    std::string cvv;
    input_stream_ << "\n";

    ui.PromptCardCvv(error_msg, cvv);

    EXPECT_NE(output_stream_.str().find(UIStrings::CARD_CVV_PROMPT), std::string::npos);
    EXPECT_EQ(cvv, "");
}

TEST_F(UITest, PromptCardCvv_InputWithErrorMessage) {
    UI ui;
    std::string error_msg = "ERR: Test Error!";
    std::string cvv;
    input_stream_ << "211\n";

    ui.PromptCardCvv(error_msg, cvv);

    std::string output = output_stream_.str();
    EXPECT_NE(output_stream_.str().find(UIStrings::CARD_CVV_PROMPT), std::string::npos);
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

    EXPECT_NE(output_stream_.str().find(UIStrings::CARD_MONTH_PROMPT), std::string::npos);
    EXPECT_EQ(month, "10");
}

TEST_F(UITest, PromptCardMonth_InputEmptyNumber) {
    UI ui;
    std::string error_msg;
    std::string month;
    input_stream_ << "\n";

    ui.PromptCardMonth(error_msg, month);

    EXPECT_NE(output_stream_.str().find(UIStrings::CARD_MONTH_PROMPT), std::string::npos);
    EXPECT_EQ(month, "");
}

TEST_F(UITest, PromptCardMonth_InputWithErrorMessage) {
    UI ui;
    std::string error_msg = "ERR: Test Error!";
    std::string month;
    input_stream_ << "11\n";

    ui.PromptCardMonth(error_msg, month);

    std::string output = output_stream_.str();
    EXPECT_NE(output_stream_.str().find(UIStrings::CARD_MONTH_PROMPT), std::string::npos);
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

    EXPECT_NE(output_stream_.str().find(UIStrings::CARD_NAME_PROMPT), std::string::npos);
    EXPECT_EQ(name, "Venture One");
}

TEST_F(UITest, PromptCardName_InputEmptyName) {
    UI ui;
    std::string error_msg;
    std::string name;
    input_stream_ << "\n";

    ui.PromptCardName(error_msg, name);

    EXPECT_NE(output_stream_.str().find(UIStrings::CARD_NAME_PROMPT), std::string::npos);
    EXPECT_EQ(name, "");
}

TEST_F(UITest, PromptCardName_InputWithErrorMessage) {
    UI ui;
    std::string error_msg = "ERR: Test Error!";
    std::string name;
    input_stream_ << "BofA Cash\n";

    ui.PromptCardName(error_msg, name);

    std::string output = output_stream_.str();
    EXPECT_NE(output_stream_.str().find(UIStrings::CARD_NAME_PROMPT), std::string::npos);
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

    EXPECT_NE(output_stream_.str().find(UIStrings::CARD_NUMBER_PROMPT), std::string::npos);
    EXPECT_EQ(number, "1111111111111111");
}

TEST_F(UITest, PromptCardNumber_InputEmptyNumber) {
    UI ui;
    std::string error_msg;
    std::string number;
    input_stream_ << "\n";

    ui.PromptCardNumber(error_msg, number);

    EXPECT_NE(output_stream_.str().find(UIStrings::CARD_NUMBER_PROMPT), std::string::npos);
    EXPECT_EQ(number, "");
}

TEST_F(UITest, PromptCardNumber_InputWithErrorMessage) {
    UI ui;
    std::string error_msg = "ERR: Test Error!";
    std::string number;
    input_stream_ << "1111111111111111\n";

    ui.PromptCardNumber(error_msg, number);

    std::string output = output_stream_.str();
    EXPECT_NE(output.find(UIStrings::CARD_NUMBER_PROMPT), std::string::npos);
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

    EXPECT_NE(output_stream_.str().find(UIStrings::CARD_YEAR_PROMPT), std::string::npos);
    EXPECT_EQ(year, "1000");
}

TEST_F(UITest, PromptCardYear_InputEmptyYear) {
    UI ui;
    std::string error_msg;
    std::string year;
    input_stream_ << "\n";

    ui.PromptCardYear(error_msg, year);

    EXPECT_NE(output_stream_.str().find(UIStrings::CARD_YEAR_PROMPT), std::string::npos);
    EXPECT_EQ(year, "");
}

TEST_F(UITest, PromptCardYear_InputWithErrorMessage) {
    UI ui;
    std::string error_msg = "ERR: Test Error!";
    std::string year;
    input_stream_ << "1000\n";

    ui.PromptCardYear(error_msg, year);

    std::string output = output_stream_.str();
    EXPECT_NE(output.find(UIStrings::CARD_YEAR_PROMPT), std::string::npos);
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
    EXPECT_NE(output.find(UIStrings::PASSWORD_PROMPT), std::string::npos);
    EXPECT_EQ(output.find("s3cr3tP@ss"),
              std::string::npos); // Shouldn't output password
    EXPECT_EQ(password, "s3cr3tP@ss");
}

TEST_F(UITest, PromptLogin_EmptyPassword) {
    UI ui;
    std::string password;
    input_stream_ << "\n";

    ui.PromptLogin(password);

    EXPECT_NE(output_stream_.str().find(UIStrings::PASSWORD_PROMPT), std::string::npos);
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
    EXPECT_TRUE(output.find(UIStrings::CONFIRMATION_CANCEL) != std::string::npos);
    EXPECT_TRUE(output.find(UIStrings::CONFIRMATION_CONFIRM) != std::string::npos);
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
    EXPECT_NE(output_stream_.str().find(UIStrings::REQUEST_VALID_INPUT), std::string::npos);
}

TEST_F(UITest, GetSelection_InvalidNegativeNumericInput) {
    UI ui;
    input_stream_ << "-3\n1\n";

    int result = TestGetSelection(ui, 0, 1);

    EXPECT_EQ(result, 1);
    EXPECT_NE(output_stream_.str().find(UIStrings::REQUEST_VALID_INPUT), std::string::npos);
}

TEST_F(UITest, GetSelection_InvalidTextInput) {
    UI ui;
    input_stream_ << "abcabc\n1\n";

    int result = TestGetSelection(ui, 0, 1);

    EXPECT_EQ(result, 1);
    EXPECT_NE(output_stream_.str().find(UIStrings::REQUEST_VALID_INPUT), std::string::npos);
}

TEST_F(UITest, GetSelection_EmptyInput) {
    UI ui;
    input_stream_ << "\n1\n";

    int result = TestGetSelection(ui, 0, 1);

    EXPECT_EQ(result, 1);
    EXPECT_NE(output_stream_.str().find(UIStrings::REQUEST_VALID_INPUT), std::string::npos);
}
