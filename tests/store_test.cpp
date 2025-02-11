#include "icryptomock.hpp"
#include "sodiumcrypto.hpp"
#include "store.hpp"

#include <fstream>
#include <gtest/test.h>
#include <memory>

using std::make_unique;

class StoreTest : public ::testing::Test {
  protected:
    void SetUp() override {
    }

    void TearDown() override {
    }

    Store store_;
};

