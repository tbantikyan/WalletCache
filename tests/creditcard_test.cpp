#include "creditcard.hpp"

#include <gtest/gtest.h>

class CreditCardTest : public ::testing::Test {
  protected:
    string TEST_GetNetworkString(CreditCard &card) {
        return card.GetNetworkString(); // Access via friend
    }
};

// SetName
TEST_F(CreditCardTest, SetName_ValidAlnum_0) {
    CreditCard card;
    ASSERT_EQ(card.SetName("JohnDoe123"), 0);
}

TEST_F(CreditCardTest, SetName_InvalidChars_Negative1) {
    CreditCard card;
    ASSERT_EQ(card.SetName("John@Doe"), -1);
}

TEST_F(CreditCardTest, SetName_Empty_0) {
    CreditCard card;
    ASSERT_EQ(card.SetName(""), 0);
}

// SetCardNumber
TEST_F(CreditCardTest, SetCardNumber_ValidDigitsAndLuhn_0) {
    CreditCard card;
    ASSERT_EQ(card.SetCardNumber("4111111111111111"), 0);
}

TEST_F(CreditCardTest, SetCardNumber_InvalidDigits_Negative1) {
    CreditCard card;
    ASSERT_EQ(card.SetCardNumber("4111-1111-1111"), -1);
}

TEST_F(CreditCardTest, SetCardNumber_ValidDigitsInvalidLuhn_Negative1) {
    CreditCard card;
    ASSERT_EQ(card.SetCardNumber("4111111111111112"), -1);
}

TEST_F(CreditCardTest, SetCardNumber_EmptyInput_Negative1) {
    CreditCard card;
    ASSERT_EQ(card.SetCardNumber(""), -1);
}

// SetCvv
TEST_F(CreditCardTest, SetCvv_Amex4Digits_0) {
    CreditCard card;
    card.SetCardNumber("371046275845869");
    ASSERT_EQ(card.SetCvv("1234"), 0);
}

TEST_F(CreditCardTest, SetCvv_Amex3Digits_Negative1) {
    CreditCard card;
    card.SetCardNumber("371046275845869");
    ASSERT_EQ(card.SetCvv("123"), -1);
}

TEST_F(CreditCardTest, SetCvv_Mastercard3Digits_0) {
    CreditCard card;
    card.SetCardNumber("5555555555554444");
    ASSERT_EQ(card.SetCvv("123"), 0);
}

TEST_F(CreditCardTest, SetCvv_Mastercard4Digits_Negative1) {
    CreditCard card;
    card.SetCardNumber("5555555555554444");
    ASSERT_EQ(card.SetCvv("1234"), -1);
}

// SetMonth
TEST_F(CreditCardTest, SetMonth_ValidMonth_0) {
    CreditCard card;
    ASSERT_EQ(card.SetMonth("12"), 0);
}

TEST_F(CreditCardTest, SetMonth_InvalidMonth_Negative1) {
    CreditCard card;
    ASSERT_EQ(card.SetMonth("13"), -1);
    ASSERT_EQ(card.SetMonth("0"), -1);
}

// SetYear
TEST_F(CreditCardTest, SetYear_ValidYear_0) {
    CreditCard card;
    ASSERT_EQ(card.SetYear("2025"), 0);
}

TEST_F(CreditCardTest, SetYear_InvalidYear_Negative1) {
    CreditCard card;
    ASSERT_EQ(card.SetYear("1899"), -1);
}

// GetName
TEST_F(CreditCardTest, GetName_NameSet) {
    CreditCard card;
    card.SetName("Travel");
    ASSERT_EQ(card.GetName(), "Travel");
}

TEST_F(CreditCardTest, GetName_NameEmptyCardSet_DefaultName) {
    CreditCard card;
    card.SetCardNumber("4111111111111111");
    ASSERT_EQ(card.GetName(), "Visa 1111");
}

TEST_F(CreditCardTest, GetName_NameAndCardEmpty_Empty) {
    CreditCard card;
    ASSERT_EQ(card.GetName(), "");
}

TEST_F(CreditCardTest, GetName_NameResetToEmpty_ReturnsDefault) {
    CreditCard card;
    card.SetCardNumber("4111111111111111");
    card.SetName("Restaurants");
    ASSERT_EQ(card.GetName(), "Restaurants");
    card.SetName("");
    ASSERT_EQ(card.GetName(), "Visa 1111");
}

// FormatText
TEST_F(CreditCardTest, FormatText_AllFieldsFilled_FormattedString) {
    CreditCard card;
    card.SetName("BCE");
    card.SetCardNumber("4111111111111111");
    card.SetCvv("123");
    card.SetMonth("12");
    card.SetYear("2025");
    ASSERT_EQ(card.FormatText(), "BCE,4111111111111111,123,12,2025;");
}

TEST_F(CreditCardTest, FormatText_NameEmpty_FormattedWithoutName) {
    CreditCard card;
    card.SetCardNumber("4111111111111111");
    card.SetCvv("123");
    card.SetMonth("12");
    card.SetYear("2025");
    ASSERT_EQ(card.FormatText(), ",4111111111111111,123,12,2025;");
}

// InitFromText
TEST_F(CreditCardTest, InitFromText_WithName) {
    CreditCard card;
    char text[] = "CSR,4111111111111111,123,12,2025";
    card.InitFromText(text);
    ASSERT_EQ(card.FormatText(), "CSR,4111111111111111,123,12,2025;");
}

TEST_F(CreditCardTest, InitFromText_WithoutName) {
    CreditCard card;
    char text[] = ",371046275845869,1234,12,2025";
    card.InitFromText(text);
    ASSERT_EQ(card.FormatText(), ",371046275845869,1234,12,2025;");
}

// GetNetworkString
TEST_F(CreditCardTest, GetNetworkString_VisaCard_Visa) {
    CreditCard card;
    card.SetCardNumber("4111111111111111");
    ASSERT_EQ(TEST_GetNetworkString(card), "Visa");
}

TEST_F(CreditCardTest, GetNetworkString_Mastercard2Card_Mastercard) {
    CreditCard card;
    card.SetCardNumber("2439331222445302");
    ASSERT_EQ(TEST_GetNetworkString(card), "Mastercard");
}

TEST_F(CreditCardTest, GetNetworkString_Mastercard5Card_Mastercard) {
    CreditCard card;
    card.SetCardNumber("5555555555554444");
    ASSERT_EQ(TEST_GetNetworkString(card), "Mastercard");
}

TEST_F(CreditCardTest, GetNetworkString_AmexCard_AmericanExpress) {
    CreditCard card;
    card.SetCardNumber("371046275845869");
    ASSERT_EQ(TEST_GetNetworkString(card), "American Express");
}

TEST_F(CreditCardTest, GetNetworkString_DiscoverCard_Discover) {
    CreditCard card;
    card.SetCardNumber("6011111111111117");
    ASSERT_EQ(TEST_GetNetworkString(card), "Discover");
}

TEST_F(CreditCardTest, GetNetworkString_OtherCard_Other) {
    CreditCard card;
    card.SetCardNumber("8252624630862584");
    ASSERT_EQ(TEST_GetNetworkString(card), "Other");
}
