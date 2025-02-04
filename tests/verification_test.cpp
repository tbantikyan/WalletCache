#include "verification.hpp"

#include <gtest/gtest.h>

// ValidateCreditCardNumber
TEST(VerificationTest, ValidateCreditCardNumber_EmptyString_False) { ASSERT_FALSE(ValidateCreditCardNumber("")); }

TEST(VerificationTest, ValidateCreditCardNumber_SmallNumber_False) { ASSERT_FALSE(ValidateCreditCardNumber("234")); }

TEST(VerificationTest, ValidateCreditCardNumber_InvalidCardNumber_False) {
    ASSERT_FALSE(ValidateCreditCardNumber("1234123412341234"));
}

TEST(VerificationTest, ValidateCreditCardNumber_ValidCardNumber_True) {
    ASSERT_TRUE(ValidateCreditCardNumber("5309024080252296"));
    ASSERT_TRUE(ValidateCreditCardNumber("344218402904125"));
}

// ValidateInputAlnumOnly
TEST(VerificationTest, ValidateInputAlnumOnly_EmptyString_False) { ASSERT_FALSE(ValidateInputDigitsOnly("")); }

TEST(VerificationTest, ValidateInputAlnumOnly_Zero_True) { ASSERT_TRUE(ValidateInputAlnumOnly("0")); }

TEST(VerificationTest, ValidateInputAlnumOnly_LargeNum_True) {
    ASSERT_TRUE(ValidateInputAlnumOnly("34536346344323590235"));
}

TEST(VerificationTest, ValidateInputAlnumOnly_NegativeThree_False) { ASSERT_FALSE(ValidateInputAlnumOnly("-3")); }

TEST(VerificationTest, ValidateInputAlnumOnly_Letters_True) { ASSERT_TRUE(ValidateInputAlnumOnly("abc")); }

TEST(VerificationTest, ValidateInputAlnumOnly_LettersAndNumbers_True) { ASSERT_TRUE(ValidateInputAlnumOnly("abc123")); }

TEST(VerificationTest, ValidateInputAlnumOnly_LettersAndSymbols_False) { ASSERT_FALSE(ValidateInputAlnumOnly("abc!")); }

// ValidateInputDigitsOnly
TEST(VerificationTest, ValidateInputDigitsOnly_EmptyString_False) { ASSERT_FALSE(ValidateInputDigitsOnly("")); }

TEST(VerificationTest, ValidateInputDigitsOnly_Zero_True) { ASSERT_TRUE(ValidateInputDigitsOnly("0")); }

TEST(VerificationTest, ValidateInputDigitsOnly_LargeNum_True) {
    ASSERT_TRUE(ValidateInputDigitsOnly("34536346344323590235"));
}

TEST(VerificationTest, ValidateInputDigitsOnly_NegativeThree_False) { ASSERT_FALSE(ValidateInputDigitsOnly("-3")); }

TEST(VerificationTest, ValidateInputDigitsOnly_Letters_False) { ASSERT_FALSE(ValidateInputDigitsOnly("abc")); }

// ValidateInputInRange
TEST(VerificationTest, ValidateInputInRange_ZeroExpectingZeroToOne_True) {
    ASSERT_TRUE(ValidateInputInRange("0", 0, 1));
}

TEST(VerificationTest, ValidateInputInRange_ThreeExpectingZeroToFive_True) {
    ASSERT_TRUE(ValidateInputInRange("3", 0, 5));
}

TEST(VerificationTest, ValidateInputInRange_Neg1ExpectingZeroToOne_False) {
    ASSERT_FALSE(ValidateInputInRange("-1", 0, 1));
}

TEST(VerificationTest, ValidateInputInRange_EightExpectingZeroToSix_False) {
    ASSERT_FALSE(ValidateInputInRange("8", 0, 6));
}

TEST(VerificationTest, ValidateInputInRange_WordExpectingZeroToOne_False) {
    ASSERT_FALSE(ValidateInputInRange("other", 0, 1));
}

// ValidateNewPassword
TEST(VerificationTest, ValidateNewPassword_MatchingPasswords_Zero) {
    string str1(MIN_PASSWORD_LENGTH, 'A');
    string str2(MIN_PASSWORD_LENGTH, 'A');
    ASSERT_EQ(VerifyNewPassword(str1, str2), 0);
}

TEST(VerificationTest, ValidateNewPassword_DifferentPasswords_PASS_NO_MATCH) {
    string str1(MIN_PASSWORD_LENGTH, 'A');
    string str2(MIN_PASSWORD_LENGTH, 'a');
    ASSERT_EQ(VerifyNewPassword(str1, str2), PASS_NO_MATCH);
}

TEST(VerificationTest, ValidateNewPassword_ShortMatchingPasswords_PASS_TOO_SHORT) {
    string str1(MIN_PASSWORD_LENGTH - 1, 'A');
    string str2(MIN_PASSWORD_LENGTH - 1, 'A');
    ASSERT_EQ(VerifyNewPassword(str1, str2), PASS_TOO_SHORT);
}

TEST(VerificationTest, ValidateNewPassword_ShortConfirmPasswords_PASS_NO_MATCH) {
    string str1(MIN_PASSWORD_LENGTH, 'A');
    string str2(MIN_PASSWORD_LENGTH - 1, 'A');
    ASSERT_EQ(VerifyNewPassword(str1, str2), PASS_NO_MATCH);
}

TEST(VerificationTest, ValidateNewPassword_ShortPassword_PASS_TOO_SHORT) {
    string str1(MIN_PASSWORD_LENGTH - 1, 'A');
    string str2(MIN_PASSWORD_LENGTH, 'A');
    ASSERT_EQ(VerifyNewPassword(str1, str2), PASS_TOO_SHORT);
}

TEST(VerificationTest, ValidateNewPassword_LongMatchingPasswords_PASS_TOO_LONG) {
    string str1(MAX_PASSWORD_LENGTH + 1, 'A');
    string str2(MAX_PASSWORD_LENGTH + 1, 'A');
    ASSERT_EQ(VerifyNewPassword(str1, str2), PASS_TOO_LONG);
}

TEST(VerificationTest, ValidateNewPassword_LongConfirmPasswords_PASS_NO_MATCH) {
    string str1(MAX_PASSWORD_LENGTH, 'A');
    string str2(MAX_PASSWORD_LENGTH + 1, 'A');
    ASSERT_EQ(VerifyNewPassword(str1, str2), PASS_NO_MATCH);
}

TEST(VerificationTest, ValidateNewPassword_LongPassword_PASS_TOO_LONG) {
    string str1(MAX_PASSWORD_LENGTH + 1, 'A');
    string str2(MAX_PASSWORD_LENGTH, 'A');
    ASSERT_EQ(VerifyNewPassword(str1, str2), PASS_TOO_LONG);
}
