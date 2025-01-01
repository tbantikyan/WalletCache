#include "utils.hpp"

#include <gtest/gtest.h>

// validate_input_int
TEST(UtilsTest, validate_input_int_ZeroExpectingZeroToOne_True) {
    ASSERT_TRUE(validate_input_int("0", 0, 1));
}

TEST(UtilsTest, validate_input_int_ThreeExpectingZeroToFive_True) {
    ASSERT_TRUE(validate_input_int("3", 0, 5));
}

TEST(UtilsTest, validate_input_int_Neg1ExpectingZeroToOne_False) {
    ASSERT_FALSE(validate_input_int("-1", 0, 1));
}

TEST(UtilsTest, validate_input_int_EightExpectingZeroToSix_False) {
    ASSERT_FALSE(validate_input_int("8", 0, 6));
}

TEST(UtilsTest, validate_input_int_WordExpectingZeroToOne_False) {
    ASSERT_FALSE(validate_input_int("other", 0, 1));
}

