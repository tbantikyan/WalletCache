#include "creditcard.hpp"

#include <gtest/gtest.h>

class CreditCardTest : public ::testing::Test {
  protected:
    auto TestGetNetworkString(CreditCard &card) -> std::string {
        return card.GetNetworkString(); // Access via friend
    }
};

// SetName
TEST_F(CreditCardTest, SetName_ValidAlnum_Returns0) {
    CreditCard card;
    EXPECT_EQ(card.SetName("JohnDoe123"), 0);
}

TEST_F(CreditCardTest, SetName_InvalidChars_ReturnsNegative1) {
    CreditCard card;
    EXPECT_EQ(card.SetName("John@Doe"), -1);
}

TEST_F(CreditCardTest, SetName_Empty_Returns0) {
    CreditCard card;
    EXPECT_EQ(card.SetName(""), 0);
}

// SetCardNumber
TEST_F(CreditCardTest, SetCardNumber_ValidDigitsAndLuhn_Returns0) {
    CreditCard card;
    EXPECT_EQ(card.SetCardNumber("4111111111111111"), 0);
    EXPECT_EQ(card.GetName(), "Visa 1111");
}

TEST_F(CreditCardTest, SetCardNumber_InvalidDigits_ReturnsNegative1) {
    CreditCard card;
    EXPECT_EQ(card.SetCardNumber("4111-1111-1111"), -1);
}

TEST_F(CreditCardTest, SetCardNumber_ValidDigitsInvalidLuhn_ReturnsNegative1) {
    CreditCard card;
    EXPECT_EQ(card.SetCardNumber("4111111111111112"), -1);
}

TEST_F(CreditCardTest, SetCardNumber_EmptyInput_ReturnsNegative1) {
    CreditCard card;
    EXPECT_EQ(card.SetCardNumber(""), -1);
}

// SetCvv
TEST_F(CreditCardTest, SetCvv_Amex4Digits_Returns0) {
    CreditCard card;
    card.SetCardNumber("371046275845869");
    EXPECT_EQ(card.SetCvv("1234"), 0);
}

TEST_F(CreditCardTest, SetCvv_Amex3Digits_ReturnsNegative1) {
    CreditCard card;
    card.SetCardNumber("371046275845869");
    EXPECT_EQ(card.SetCvv("123"), -1);
}

TEST_F(CreditCardTest, SetCvv_Mastercard3Digits_Returns0) {
    CreditCard card;
    card.SetCardNumber("5555555555554444");
    EXPECT_EQ(card.SetCvv("123"), 0);
}

TEST_F(CreditCardTest, SetCvv_Mastercard4Digits_ReturnsNegative1) {
    CreditCard card;
    card.SetCardNumber("5555555555554444");
    EXPECT_EQ(card.SetCvv("1234"), -1);
}

// SetMonth
TEST_F(CreditCardTest, SetMonth_ValidMonth_Returns0) {
    CreditCard card;
    EXPECT_EQ(card.SetMonth("12"), 0);
}

TEST_F(CreditCardTest, SetMonth_InvalidMonth_ReturnsNegative1) {
    CreditCard card;
    EXPECT_EQ(card.SetMonth("13"), -1);
    EXPECT_EQ(card.SetMonth("0"), -1);
}

// SetYear
TEST_F(CreditCardTest, SetYear_ValidYear_Returns0) {
    CreditCard card;
    EXPECT_EQ(card.SetYear("2025"), 0);
}

TEST_F(CreditCardTest, SetYear_InvalidYear_ReturnsNegative1) {
    CreditCard card;
    EXPECT_EQ(card.SetYear("1899"), -1);
}

// GetName
TEST_F(CreditCardTest, GetName_NameSet_ReturnsSetName) {
    CreditCard card;
    card.SetName("Travel");
    EXPECT_EQ(card.GetName(), "Travel");
}

TEST_F(CreditCardTest, GetName_NameEmptyCardSet_ReturnsDefaultName) {
    CreditCard card;
    card.SetCardNumber("5555555555554444");
    EXPECT_EQ(card.GetName(), "Mastercard 4444");
}

TEST_F(CreditCardTest, GetName_NameAndCardEmpty_Empty) {
    CreditCard card;
    EXPECT_EQ(card.GetName(), "");
}

TEST_F(CreditCardTest, GetName_NameResetToEmpty_ReturnsDefault) {
    CreditCard card;
    card.SetCardNumber("4111111111111111");
    card.SetName("Restaurants");
    EXPECT_EQ(card.GetName(), "Restaurants");
    card.SetName("");
    EXPECT_EQ(card.GetName(), "Visa 1111");
}

// FormatText
TEST_F(CreditCardTest, FormatText_AllFieldsFilled_ReturnsFormattedString) {
    CreditCard card;
    card.SetName("BCE");
    card.SetCardNumber("4111111111111111");
    card.SetCvv("123");
    card.SetMonth("12");
    card.SetYear("2025");
    EXPECT_EQ(card.FormatText(), "BCE,4111111111111111,123,12,2025;");
}

TEST_F(CreditCardTest, FormatText_NameEmpty_ReturnsFormattedWithoutName) {
    CreditCard card;
    card.SetCardNumber("4111111111111111");
    card.SetCvv("123");
    card.SetMonth("12");
    card.SetYear("2025");
    EXPECT_EQ(card.FormatText(), ",4111111111111111,123,12,2025;");
}

// InitFromText
TEST_F(CreditCardTest, InitFromText_WithName_ReturnsFormattedTextWithName) {
    CreditCard card;
    char text[] = "CSR,4111111111111111,123,12,2025";
    card.InitFromText(text);
    EXPECT_EQ(card.FormatText(), "CSR,4111111111111111,123,12,2025;");
}

TEST_F(CreditCardTest, InitFromText_WithoutName_ReturnsFormattedTextWithoutName) {
    CreditCard card;
    char text[] = ",371046275845869,1234,12,2025";
    card.InitFromText(text);
    EXPECT_EQ(card.FormatText(), ",371046275845869,1234,12,2025;");
}

// GetNetworkString
TEST_F(CreditCardTest, GetNetworkString_VisaCard_ReturnsVisa) {
    CreditCard card;
    card.SetCardNumber("4111111111111111");
    EXPECT_EQ(TestGetNetworkString(card), "Visa");
}

TEST_F(CreditCardTest, GetNetworkString_Mastercard2Card_ReturnsMastercard) {
    CreditCard card;
    card.SetCardNumber("2439331222445302");
    EXPECT_EQ(TestGetNetworkString(card), "Mastercard");
}

TEST_F(CreditCardTest, GetNetworkString_Mastercard5Card_ReturnsMastercard) {
    CreditCard card;
    card.SetCardNumber("5555555555554444");
    EXPECT_EQ(TestGetNetworkString(card), "Mastercard");
}

TEST_F(CreditCardTest, GetNetworkString_AmexCard_ReturnsAmericanExpress) {
    CreditCard card;
    card.SetCardNumber("371046275845869");
    EXPECT_EQ(TestGetNetworkString(card), "American Express");
}

TEST_F(CreditCardTest, GetNetworkString_DiscoverCard_ReturnsDiscover) {
    CreditCard card;
    card.SetCardNumber("6011111111111117");
    EXPECT_EQ(TestGetNetworkString(card), "Discover");
}

TEST_F(CreditCardTest, GetNetworkString_OtherCard_ReturnsOther) {
    CreditCard card;
    card.SetCardNumber("8252624630862584");
    EXPECT_EQ(TestGetNetworkString(card), "Other");
}

// GetFields
TEST_F(CreditCardTest, GetDisplayFields_AllFieldsFilled_ReturnsFieldsVector) {
    std::vector<std::pair<std::string, std::string>> expected_pairs = {
        {UIStrings::CARD_NAME_LABEL, "BCE"},
        {UIStrings::CARD_NUMBER_LABEL, "4111111111111111"},
        {UIStrings::CARD_CVV_LABEL, "123"},
        {UIStrings::CARD_MONTH_LABEL, "12"},
        {UIStrings::CARD_YEAR_LABEL, "2025"}};

    CreditCard card;
    card.SetName(expected_pairs[0].second);
    card.SetCardNumber(expected_pairs[1].second);
    card.SetCvv(expected_pairs[2].second);
    card.SetMonth(expected_pairs[3].second);
    card.SetYear(expected_pairs[4].second);

    CreditCardViewModel card_view;
    std::vector<std::pair<std::string, std::string>> fields = card_view.GetDisplayFields(card);
    ASSERT_EQ(expected_pairs.size(), fields.size());
    EXPECT_TRUE(std::equal(fields.begin(), fields.end(), expected_pairs.begin()));
}

TEST_F(CreditCardTest, GetDisplayFields_NameEmpty_ReturnsDefaultNameInFieldsVector) {
    std::vector<std::pair<std::string, std::string>> expected_pairs = {
        {UIStrings::CARD_NAME_LABEL, "Visa 1111"},
        {UIStrings::CARD_NUMBER_LABEL, "4111111111111111"},
        {UIStrings::CARD_CVV_LABEL, "123"},
        {UIStrings::CARD_MONTH_LABEL, "12"},
        {UIStrings::CARD_YEAR_LABEL, "2025"}};

    CreditCard card;
    card.SetCardNumber(expected_pairs[1].second);
    card.SetCvv(expected_pairs[2].second);
    card.SetMonth(expected_pairs[3].second);
    card.SetYear(expected_pairs[4].second);

    CreditCardViewModel card_view;
    std::vector<std::pair<std::string, std::string>> fields = card_view.GetDisplayFields(card);
    ASSERT_EQ(expected_pairs.size(), fields.size());
    EXPECT_TRUE(std::equal(fields.begin(), fields.end(), expected_pairs.begin()));
}
