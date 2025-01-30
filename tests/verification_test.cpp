#include "verification.hpp"

#include <gtest/gtest.h>

// validate_credit_card_number
TEST(VerificationTest, validate_credit_card_number_EmptyString_False) {
    ASSERT_FALSE(validate_credit_card_number(""));
}

TEST(VerificationTest, validate_credit_card_number_SmallNumber_False) {
    ASSERT_FALSE(validate_credit_card_number("234"));
}

TEST(VerificationTest, validate_credit_card_number_InvalidCardNumber_False) {
    ASSERT_FALSE(validate_credit_card_number("1234123412341234"));
}

TEST(VerificationTest, validate_credit_card_number_ValidCardNumber_True) {
    ASSERT_TRUE(validate_credit_card_number("5309024080252296"));
}

// validate_input_alnum_only
TEST(VerificationTest, validate_input_alnum_only_EmptyString_False) {
    ASSERT_FALSE(validate_input_digits_only(""));
}

TEST(VerificationTest, validate_input_alnum_only_Zero_True) {
    ASSERT_TRUE(validate_input_alnum_only("0"));
}

TEST(VerificationTest, validate_input_alnum_only_LargeNum_True) {
    ASSERT_TRUE(validate_input_alnum_only("34536346344323590235"));
}

TEST(VerificationTest, validate_input_alnum_only_NegativeThree_False) {
    ASSERT_FALSE(validate_input_alnum_only("-3"));
}

TEST(VerificationTest, validate_input_alnum_only_Letters_True) {
    ASSERT_TRUE(validate_input_alnum_only("abc"));
}

TEST(VerificationTest, validate_input_alnum_only_LettersAndNumbers_True) {
    ASSERT_TRUE(validate_input_alnum_only("abc123"));
}

TEST(VerificationTest, validate_input_alnum_only_LettersAndSymbols_False) {
    ASSERT_FALSE(validate_input_alnum_only("abc!"));
}

// validate_input_digits_only
TEST(VerificationTest, validate_input_digits_only_EmptyString_False) {
    ASSERT_FALSE(validate_input_digits_only(""));
}

TEST(VerificationTest, validate_input_digits_only_Zero_True) {
    ASSERT_TRUE(validate_input_digits_only("0"));
}

TEST(VerificationTest, validate_input_digits_only_LargeNum_True) {
    ASSERT_TRUE(validate_input_digits_only("34536346344323590235"));
}

TEST(VerificationTest, validate_input_digits_only_NegativeThree_False) {
    ASSERT_FALSE(validate_input_digits_only("-3"));
}

TEST(VerificationTest, validate_input_digits_only_Letters_False) {
    ASSERT_FALSE(validate_input_digits_only("abc"));
}

// validate_input_in_range
TEST(VerificationTest, validate_input_in_range_ZeroExpectingZeroToOne_True) {
    ASSERT_TRUE(validate_input_in_range("0", 0, 1));
}

TEST(VerificationTest, validate_input_in_range_ThreeExpectingZeroToFive_True) {
    ASSERT_TRUE(validate_input_in_range("3", 0, 5));
}

TEST(VerificationTest, validate_input_in_range_Neg1ExpectingZeroToOne_False) {
    ASSERT_FALSE(validate_input_in_range("-1", 0, 1));
}

TEST(VerificationTest, validate_input_in_range_EightExpectingZeroToSix_False) {
    ASSERT_FALSE(validate_input_in_range("8", 0, 6));
}

TEST(VerificationTest, validate_input_in_range_WordExpectingZeroToOne_False) {
    ASSERT_FALSE(validate_input_in_range("other", 0, 1));
}

// validate_new_password
TEST(VerificationTest, validate_new_password_MatchingPasswords_Zero) {
    string str1(MIN_PASSWORD_LENGTH, 'A');
    string str2(MIN_PASSWORD_LENGTH, 'A');
    ASSERT_EQ(verify_new_password(str1, str2), 0);
}

TEST(VerificationTest, validate_new_password_DifferentPasswords_PASS_NO_MATCH) {
    string str1(MIN_PASSWORD_LENGTH, 'A');
    string str2(MIN_PASSWORD_LENGTH, 'a');
    ASSERT_EQ(verify_new_password(str1, str2), PASS_NO_MATCH);
}

TEST(VerificationTest, validate_new_password_ShortMatchingPasswords_PASS_TOO_SHORT) {
    string str1(MIN_PASSWORD_LENGTH - 1, 'A');
    string str2(MIN_PASSWORD_LENGTH - 1, 'A');
    ASSERT_EQ(verify_new_password(str1, str2), PASS_TOO_SHORT);
}

TEST(VerificationTest, validate_new_password_ShortConfirmPasswords_PASS_NO_MATCH) {
    string str1(MIN_PASSWORD_LENGTH, 'A');
    string str2(MIN_PASSWORD_LENGTH - 1, 'A');
    ASSERT_EQ(verify_new_password(str1, str2), PASS_NO_MATCH);
}

TEST(VerificationTest, validate_new_password_ShortPassword_PASS_TOO_SHORT) {
    string str1(MIN_PASSWORD_LENGTH - 1, 'A');
    string str2(MIN_PASSWORD_LENGTH, 'A');
    ASSERT_EQ(verify_new_password(str1, str2), PASS_TOO_SHORT);
}

TEST(VerificationTest, validate_new_password_LongMatchingPasswords_PASS_TOO_LONG) {
    string str1(MAX_PASSWORD_LENGTH + 1, 'A');
    string str2(MAX_PASSWORD_LENGTH + 1, 'A');
    ASSERT_EQ(verify_new_password(str1, str2), PASS_TOO_LONG);
}

TEST(VerificationTest, validate_new_password_LongConfirmPasswords_PASS_NO_MATCH) {
    string str1(MAX_PASSWORD_LENGTH, 'A');
    string str2(MAX_PASSWORD_LENGTH + 1, 'A');
    ASSERT_EQ(verify_new_password(str1, str2), PASS_NO_MATCH);
}

TEST(VerificationTest, validate_new_password_LongPassword_PASS_TOO_LONG) {
    string str1(MAX_PASSWORD_LENGTH + 1, 'A');
    string str2(MAX_PASSWORD_LENGTH, 'A');
    ASSERT_EQ(verify_new_password(str1, str2), PASS_TOO_LONG);
}
