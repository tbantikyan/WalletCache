#include "creditcard.hpp"

#include <gtest/gtest.h>

class CreditCardTest : public ::testing::Test {
  protected:
    string TEST_get_network_string(CreditCard &card) {
        return card.get_network_string(); // Access via friend
    }
};

// set_name
TEST_F(CreditCardTest, set_name_ValidAlnum_0) {
    CreditCard card;
    ASSERT_EQ(card.set_name("JohnDoe123"), 0);
}

TEST_F(CreditCardTest, set_name_InvalidChars_Negative1) {
    CreditCard card;
    ASSERT_EQ(card.set_name("John@Doe"), -1);
}

TEST_F(CreditCardTest, set_name_Empty_0) {
    CreditCard card;
    ASSERT_EQ(card.set_name(""), 0);
}

// set_card_number
TEST_F(CreditCardTest, set_card_number_ValidDigitsAndLuhn_0) {
    CreditCard card;
    ASSERT_EQ(card.set_card_number("4111111111111111"), 0);
}

TEST_F(CreditCardTest, set_card_number_InvalidDigits_Negative1) {
    CreditCard card;
    ASSERT_EQ(card.set_card_number("4111-1111-1111"), -1);
}

TEST_F(CreditCardTest, set_card_number_ValidDigitsInvalidLuhn_Negative1) {
    CreditCard card;
    ASSERT_EQ(card.set_card_number("4111111111111112"), -1);
}

TEST_F(CreditCardTest, set_card_number_EmptyInput_Negative1) {
    CreditCard card;
    ASSERT_EQ(card.set_card_number(""), -1);
}

// set_cvv
TEST_F(CreditCardTest, set_cvv_Amex4Digits_0) {
    CreditCard card;
    card.set_card_number("371046275845869");
    ASSERT_EQ(card.set_cvv("1234"), 0);
}

TEST_F(CreditCardTest, set_cvv_Amex3Digits_Negative1) {
    CreditCard card;
    card.set_card_number("371046275845869");
    ASSERT_EQ(card.set_cvv("123"), -1);
}

TEST_F(CreditCardTest, set_cvv_Mastercard3Digits_0) {
    CreditCard card;
    card.set_card_number("5555555555554444");
    ASSERT_EQ(card.set_cvv("123"), 0);
}

TEST_F(CreditCardTest, set_cvv_Mastercard4Digits_Negative1) {
    CreditCard card;
    card.set_card_number("5555555555554444");
    ASSERT_EQ(card.set_cvv("1234"), -1);
}

// set_month
TEST_F(CreditCardTest, set_month_ValidMonth_0) {
    CreditCard card;
    ASSERT_EQ(card.set_month("12"), 0);
}

TEST_F(CreditCardTest, set_month_InvalidMonth_Negative1) {
    CreditCard card;
    ASSERT_EQ(card.set_month("13"), -1);
    ASSERT_EQ(card.set_month("0"), -1);
}

// set_year
TEST_F(CreditCardTest, set_year_ValidYear_0) {
    CreditCard card;
    ASSERT_EQ(card.set_year("2025"), 0);
}

TEST_F(CreditCardTest, set_year_InvalidYear_Negative1) {
    CreditCard card;
    ASSERT_EQ(card.set_year("1899"), -1);
}

// get_name
TEST_F(CreditCardTest, get_name_NameSet) {
    CreditCard card;
    card.set_name("Travel");
    ASSERT_EQ(card.get_name(), "Travel");
}

TEST_F(CreditCardTest, get_name_NameEmptyCardSet_DefaultName) {
    CreditCard card;
    card.set_card_number("4111111111111111");
    ASSERT_EQ(card.get_name(), "Visa 1111");
}

TEST_F(CreditCardTest, get_name_NameAndCardEmpty_Empty) {
    CreditCard card;
    ASSERT_EQ(card.get_name(), "");
}

TEST_F(CreditCardTest, get_name_NameResetToEmpty_ReturnsDefault) {
    CreditCard card;
    card.set_card_number("4111111111111111");
    card.set_name("Restaurants");
    ASSERT_EQ(card.get_name(), "Restaurants");
    card.set_name("");
    ASSERT_EQ(card.get_name(), "Visa 1111");
}

// format_text
TEST_F(CreditCardTest, format_text_AllFieldsFilled_FormattedString) {
    CreditCard card;
    card.set_name("BCE");
    card.set_card_number("4111111111111111");
    card.set_cvv("123");
    card.set_month("12");
    card.set_year("2025");
    ASSERT_EQ(card.format_text(), "BCE,4111111111111111,123,12,2025;");
}

TEST_F(CreditCardTest, format_text_NameEmpty_FormattedWithoutName) {
    CreditCard card;
    card.set_card_number("4111111111111111");
    card.set_cvv("123");
    card.set_month("12");
    card.set_year("2025");
    ASSERT_EQ(card.format_text(), ",4111111111111111,123,12,2025;");
}

// init_from_text
TEST_F(CreditCardTest, init_from_text_WithName) {
    CreditCard card;
    char text[] = "CSR,4111111111111111,123,12,2025";
    card.init_from_text(text);
    ASSERT_EQ(card.format_text(), "CSR,4111111111111111,123,12,2025;");
}

TEST_F(CreditCardTest, init_from_text_WithoutName) {
    CreditCard card;
    char text[] = ",371046275845869,1234,12,2025";
    card.init_from_text(text);
    ASSERT_EQ(card.format_text(), ",371046275845869,1234,12,2025;");
}

// get_network_string
TEST_F(CreditCardTest, get_network_string_VisaCard_Visa) {
    CreditCard card;
    card.set_card_number("4111111111111111");
    ASSERT_EQ(TEST_get_network_string(card), "Visa");
}

TEST_F(CreditCardTest, get_network_string_Mastercard2_Mastercard) {
    CreditCard card;
    card.set_card_number("2439331222445302");
    ASSERT_EQ(TEST_get_network_string(card), "Mastercard");
}

TEST_F(CreditCardTest, get_network_string_Mastercard5_Mastercard) {
    CreditCard card;
    card.set_card_number("5555555555554444");
    ASSERT_EQ(TEST_get_network_string(card), "Mastercard");
}

TEST_F(CreditCardTest, get_network_string_Amex_AmericanExpress) {
    CreditCard card;
    card.set_card_number("371046275845869");
    ASSERT_EQ(TEST_get_network_string(card), "American Express");
}

TEST_F(CreditCardTest, get_network_string_Discover_Discover) {
    CreditCard card;
    card.set_card_number("6011111111111117");
    ASSERT_EQ(TEST_get_network_string(card), "Discover");
}

TEST_F(CreditCardTest, get_network_string_Other_Other) {
    CreditCard card;
    card.set_card_number("8252624630862584");
    ASSERT_EQ(TEST_get_network_string(card), "Other");
}
