#include "verification.hpp"

#include <gtest/gtest.h>

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
    string str1(MIN_PASSWORD_LENGTH-1, 'A');
    string str2(MIN_PASSWORD_LENGTH-1, 'A');
    ASSERT_EQ(verify_new_password(str1, str2), PASS_TOO_SHORT);
}

TEST(VerificationTest, validate_new_password_ShortConfirmPasswords_PASS_NO_MATCH) {
    string str1(MIN_PASSWORD_LENGTH, 'A');
    string str2(MIN_PASSWORD_LENGTH-1, 'A');
    ASSERT_EQ(verify_new_password(str1, str2), PASS_NO_MATCH);
}

TEST(VerificationTest, validate_new_password_ShortPassword_PASS_TOO_SHORT) {
    string str1(MIN_PASSWORD_LENGTH-1, 'A');
    string str2(MIN_PASSWORD_LENGTH, 'A');
    ASSERT_EQ(verify_new_password(str1, str2), PASS_TOO_SHORT);
}

TEST(VerificationTest, validate_new_password_LongMatchingPasswords_PASS_TOO_LONG) {
    string str1(MAX_PASSWORD_LENGTH+1, 'A');
    string str2(MAX_PASSWORD_LENGTH+1, 'A');
    ASSERT_EQ(verify_new_password(str1, str2), PASS_TOO_LONG);
}

TEST(VerificationTest, validate_new_password_LongConfirmPasswords_PASS_NO_MATCH) {
    string str1(MAX_PASSWORD_LENGTH, 'A');
    string str2(MAX_PASSWORD_LENGTH+1, 'A');
    ASSERT_EQ(verify_new_password(str1, str2), PASS_NO_MATCH);
}

TEST(VerificationTest, validate_new_password_LongPassword_PASS_TOO_LONG) {
    string str1(MAX_PASSWORD_LENGTH+1, 'A');
    string str2(MAX_PASSWORD_LENGTH, 'A');
    ASSERT_EQ(verify_new_password(str1, str2), PASS_TOO_LONG);
}
