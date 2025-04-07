#include "mockcrypto.hpp"
#include "mockfileio.hpp"
#include "store.hpp"

#include <gmock/gmock.h>
#include <gtest/gtest.h>

using ::testing::_;
using ::testing::Return;

class StoreTest : public ::testing::Test {
  protected:
    MockCrypto *mock_crypto_ptr_;
    MockFileIO *mock_file_io_ptr_;
    std::unique_ptr<Store> store_;

    uint64_t hash_len_ = 32;
    uint64_t salt_len_ = 16;
    uint64_t encryption_key_len_ = 64;
    uint64_t encryption_header_len_ = 32;
    uint64_t encryption_added_bytes_ = 32;

    std::string one_card_formatted_ = ",4111111111111111,111,10,2020;";
    std::string two_cards_formatted_ = ",4111111111111111,111,10,2020;,4111111111111111,111,10,2020;";

    void SetUp() override {
        auto mock_crypto = std::make_shared<::testing::NaggyMock<MockCrypto>>();
        auto mock_file_io = std::make_unique<::testing::NaggyMock<MockFileIO>>();

        mock_crypto_ptr_ = mock_crypto.get();
        mock_file_io_ptr_ = mock_file_io.get();

        store_ = std::make_unique<Store>(mock_crypto, std::move(mock_file_io));
    }

    auto TestReadHeader(unsigned char *hash, unsigned char *salt) -> int { return store_->ReadHeader(hash, salt); }
    auto TestReadData(unsigned char *decrypted_data, uintmax_t data_size, uint64_t *decrypted_size_actual) -> int {
        return store_->ReadData(decrypted_data, data_size, decrypted_size_actual);
    }
    auto TestWriteHeader(const unsigned char *hash, const unsigned char *salt) -> int {
        return store_->WriteHeader(hash, salt);
    }
    auto TestWriteData(unsigned char *data, uintmax_t data_size) -> int { return store_->WriteData(data, data_size); }
    auto TestGetCardsSize() -> uintmax_t { return store_->GetCardsSize(); }
    auto TestCardsFormatted(unsigned char *buf) -> uintmax_t { return store_->CardsFormatted(buf); }
    void TestLoadCards(unsigned char *data) { store_->LoadCards(data); }

    inline void ValidReadHeaderExpects() {
        EXPECT_CALL(*mock_file_io_ptr_, GetPositionRead()).WillOnce(Return(0)).WillOnce(Return(hash_len_ + salt_len_));
        EXPECT_CALL(*mock_crypto_ptr_, HashLen()).WillRepeatedly(Return(hash_len_));
        EXPECT_CALL(*mock_crypto_ptr_, SaltLen()).WillRepeatedly(Return(salt_len_));
        EXPECT_CALL(*mock_file_io_ptr_, Read(_, _)).WillOnce(Return(true)).WillOnce(Return(true));
    }

    inline void ValidReadDataExpects() {
        EXPECT_CALL(*mock_crypto_ptr_, EncryptionHeaderLen()).WillRepeatedly(Return(encryption_header_len_));
        EXPECT_CALL(*mock_file_io_ptr_, Read(_, _)).WillOnce(Return(true)).WillOnce(Return(true));
        EXPECT_CALL(*mock_crypto_ptr_, DecryptBuf(_, _, _, _, _, _)).WillOnce(Return(0));
    }

    inline void ValidWriteHeaderExpects() {
        EXPECT_CALL(*mock_file_io_ptr_, GetPositionWriteTemp())
            .WillOnce(Return(0))
            .WillOnce(Return(hash_len_ + salt_len_));

        EXPECT_CALL(*mock_file_io_ptr_, WriteTemp(_, _)).WillOnce(Return(0)).WillOnce(Return(0));
        EXPECT_CALL(*mock_crypto_ptr_, HashLen()).WillRepeatedly(Return(hash_len_));
        EXPECT_CALL(*mock_crypto_ptr_, SaltLen()).WillRepeatedly(Return(salt_len_));
    }

    inline void ValidWriteDataExpects(uint64_t decrypted_data_size) {
        EXPECT_CALL(*mock_crypto_ptr_, EncryptionHeaderLen()).WillOnce(Return(encryption_header_len_));
        EXPECT_CALL(*mock_crypto_ptr_, EncryptionAddedBytes()).WillOnce(Return(encryption_header_len_));
        EXPECT_CALL(*mock_crypto_ptr_, EncryptBuf(_, _, _, _, _)).WillOnce(Return(0));

        EXPECT_CALL(*mock_file_io_ptr_, WriteTemp(_, _)).WillOnce(Return(0)).WillOnce(Return(0));

        EXPECT_CALL(*mock_crypto_ptr_, HashLen()).WillRepeatedly(Return(hash_len_));
        EXPECT_CALL(*mock_crypto_ptr_, SaltLen()).WillRepeatedly(Return(salt_len_));
        EXPECT_CALL(*mock_file_io_ptr_, GetPositionWriteTemp())
            .WillOnce(
                Return(hash_len_ + salt_len_ + encryption_header_len_ + decrypted_data_size + encryption_added_bytes_));
    }
};

// InitNewStore
TEST_F(StoreTest, InitNewStore_ValidInput_Returns0) {
    EXPECT_CALL(*mock_crypto_ptr_, HashLen()).WillRepeatedly(Return(hash_len_));
    EXPECT_CALL(*mock_crypto_ptr_, SaltLen()).WillRepeatedly(Return(salt_len_));

    EXPECT_CALL(*mock_crypto_ptr_, HashPassword(_, _)).WillOnce(Return(0));
    EXPECT_CALL(*mock_crypto_ptr_, GenerateSalt(_)).Times(1);
    EXPECT_CALL(*mock_file_io_ptr_, OpenWriteTemp()).WillOnce(Return(0));
    ValidWriteHeaderExpects();
    EXPECT_CALL(*mock_file_io_ptr_, CloseWriteTemp()).Times(1);
    EXPECT_CALL(*mock_crypto_ptr_, Memzero(_, _)).Times(1);

    unsigned char password[] = "pwd";
    EXPECT_EQ(0, store_->InitNewStore(password));
}

TEST_F(StoreTest, InitNewStore_HashPasswordFails_ReturnsNegative1) {
    EXPECT_CALL(*mock_crypto_ptr_, HashLen()).WillOnce(Return(hash_len_));
    EXPECT_CALL(*mock_crypto_ptr_, HashPassword(_, _)).WillOnce(Return(-1));

    unsigned char password[] = "pwd";
    EXPECT_EQ(store_->InitNewStore(password), -1);
}

TEST_F(StoreTest, InitNewStore_OpenWriteTempFails_ReturnsNegative1) {
    EXPECT_CALL(*mock_crypto_ptr_, HashLen()).WillRepeatedly(Return(hash_len_));

    EXPECT_CALL(*mock_crypto_ptr_, HashPassword(_, _)).WillOnce(Return(0));
    EXPECT_CALL(*mock_crypto_ptr_, SaltLen()).WillOnce(Return(salt_len_));
    EXPECT_CALL(*mock_crypto_ptr_, GenerateSalt(_)).Times(1);
    EXPECT_CALL(*mock_file_io_ptr_, OpenWriteTemp()).WillOnce(Return(-1));
    EXPECT_CALL(*mock_crypto_ptr_, Memzero(_, _)).Times(1);

    unsigned char password[] = "pwd";
    EXPECT_EQ(store_->InitNewStore(password), -1);
}

TEST_F(StoreTest, InitNewStore_WriteHeaderFails_ReturnsNegative1) {
    EXPECT_CALL(*mock_crypto_ptr_, HashLen()).WillRepeatedly(Return(hash_len_));

    EXPECT_CALL(*mock_crypto_ptr_, HashPassword(_, _)).WillOnce(Return(0));
    EXPECT_CALL(*mock_crypto_ptr_, SaltLen()).WillOnce(Return(salt_len_));
    EXPECT_CALL(*mock_crypto_ptr_, GenerateSalt(_)).Times(1);

    EXPECT_CALL(*mock_file_io_ptr_, OpenWriteTemp()).WillOnce(Return(0));
    EXPECT_CALL(*mock_file_io_ptr_, GetPositionWriteTemp())
        .WillOnce(Return(-1)); // Invalid initial position for WriteHeader
    EXPECT_CALL(*mock_crypto_ptr_, Memzero(_, _)).Times(1);
    EXPECT_CALL(*mock_file_io_ptr_, CloseWriteTemp()).Times(1);

    unsigned char password[] = "pwd";
    EXPECT_EQ(store_->InitNewStore(password), -1);
}

// LoadStore
TEST_F(StoreTest, LoadStore_NoData_ReturnsValid) {
    EXPECT_CALL(*mock_crypto_ptr_, HashLen()).WillRepeatedly(Return(hash_len_));
    EXPECT_CALL(*mock_crypto_ptr_, SaltLen()).WillRepeatedly(Return(salt_len_));
    EXPECT_CALL(*mock_crypto_ptr_, EncryptionKeyLen()).WillRepeatedly(Return(encryption_key_len_));

    EXPECT_CALL(*mock_file_io_ptr_, OpenRead()).WillOnce(Return(0));
    ValidReadHeaderExpects();
    EXPECT_CALL(*mock_crypto_ptr_, VerifyPasswordHash(_, _)).WillOnce(Return(0));
    EXPECT_CALL(*mock_crypto_ptr_, DeriveEncryptionKey(_, _, _, _)).WillOnce(Return(0));
    EXPECT_CALL(*mock_crypto_ptr_, Memzero(_, _)).Times(1);
    EXPECT_CALL(*mock_file_io_ptr_, GetSize(_)).WillOnce(Return(hash_len_ + salt_len_));
    EXPECT_CALL(*mock_file_io_ptr_, CloseRead()).Times(1);

    unsigned char password[] = "pwd";
    EXPECT_EQ(store_->LoadStore(password), Store::LOAD_STORE_VALID);
}

TEST_F(StoreTest, LoadStore_Data_ReturnsValid) {
    EXPECT_CALL(*mock_crypto_ptr_, HashLen()).WillRepeatedly(Return(hash_len_));
    EXPECT_CALL(*mock_crypto_ptr_, SaltLen()).WillRepeatedly(Return(salt_len_));
    EXPECT_CALL(*mock_crypto_ptr_, EncryptionKeyLen()).WillRepeatedly(Return(encryption_key_len_));
    EXPECT_CALL(*mock_crypto_ptr_, Memzero(_, _)).Times(2);
    EXPECT_CALL(*mock_file_io_ptr_, Read(_, _))
        .WillOnce(Return(true))
        .WillOnce(Return(true))
        .WillOnce(Return(true))
        .WillOnce(Return(true));

    EXPECT_CALL(*mock_file_io_ptr_, OpenRead()).WillOnce(Return(0));
    EXPECT_CALL(*mock_file_io_ptr_, GetPositionRead()).WillOnce(Return(0)).WillOnce(Return(hash_len_ + salt_len_));

    uintmax_t store_data_size = 200;
    EXPECT_CALL(*mock_file_io_ptr_, GetSize(_)).WillOnce(Return(store_data_size + hash_len_ + salt_len_));
    EXPECT_CALL(*mock_crypto_ptr_, EncryptionHeaderLen()).WillRepeatedly(Return(encryption_header_len_));
    EXPECT_CALL(*mock_crypto_ptr_, DecryptBuf(_, _, _, _, _, _)).WillOnce(Return(0));
    EXPECT_CALL(*mock_crypto_ptr_, VerifyPasswordHash(_, _)).WillOnce(Return(0));
    EXPECT_CALL(*mock_crypto_ptr_, DeriveEncryptionKey(_, _, _, _)).WillOnce(Return(0));
    EXPECT_CALL(*mock_file_io_ptr_, CloseRead()).Times(1);

    unsigned char password[] = "pwd";
    EXPECT_EQ(store_->LoadStore(password), Store::LOAD_STORE_VALID);
}

TEST_F(StoreTest, LoadStore_OpenReadFails_ReturnsOpenErr) {
    EXPECT_CALL(*mock_crypto_ptr_, HashLen()).WillOnce(Return(hash_len_));
    EXPECT_CALL(*mock_crypto_ptr_, SaltLen()).WillOnce(Return(salt_len_));
    EXPECT_CALL(*mock_file_io_ptr_, OpenRead()).WillOnce(Return(-1));

    unsigned char password[] = "pwd";
    EXPECT_EQ(store_->LoadStore(password), Store::LOAD_STORE_OPEN_ERR);
}

TEST_F(StoreTest, LoadStore_ReadHeaderFails_ReturnsReadErr) {
    EXPECT_CALL(*mock_crypto_ptr_, HashLen()).WillOnce(Return(hash_len_));
    EXPECT_CALL(*mock_crypto_ptr_, SaltLen()).WillOnce(Return(salt_len_));

    EXPECT_CALL(*mock_file_io_ptr_, OpenRead()).WillOnce(Return(0));
    EXPECT_CALL(*mock_file_io_ptr_, GetPositionRead).WillOnce(Return(-1)); // Invalid position ReadHeader fails
    EXPECT_CALL(*mock_file_io_ptr_, CloseRead()).Times(1);

    unsigned char password[] = "pwd";
    EXPECT_EQ(store_->LoadStore(password), Store::LOAD_STORE_HEADER_READ_ERR);
}

TEST_F(StoreTest, LoadStore_VerifyPasswordHashFails_ReturnsPwdVerifyErr) {
    // Uses WillRepeatedly because of calls in helper functions
    EXPECT_CALL(*mock_crypto_ptr_, HashLen()).WillRepeatedly(Return(hash_len_));
    EXPECT_CALL(*mock_crypto_ptr_, SaltLen()).WillRepeatedly(Return(salt_len_));

    EXPECT_CALL(*mock_file_io_ptr_, OpenRead()).WillOnce(Return(0));
    ValidReadHeaderExpects();
    EXPECT_CALL(*mock_crypto_ptr_, VerifyPasswordHash(_, _)).WillOnce(Return(-1));
    EXPECT_CALL(*mock_file_io_ptr_, CloseRead()).Times(1);

    unsigned char password[] = "pwd";
    EXPECT_EQ(store_->LoadStore(password), Store::LOAD_STORE_PWD_VERIFY_ERR);
}

TEST_F(StoreTest, LoadStore_DeriveEncryptionKeyFails_ReturnsKeyDerivationErr) {
    // Uses WillRepeatedly because of calls in helper functions
    EXPECT_CALL(*mock_crypto_ptr_, HashLen()).WillRepeatedly(Return(hash_len_));
    EXPECT_CALL(*mock_crypto_ptr_, SaltLen()).WillRepeatedly(Return(salt_len_));
    EXPECT_CALL(*mock_crypto_ptr_, EncryptionKeyLen()).WillRepeatedly(Return(encryption_key_len_));

    EXPECT_CALL(*mock_file_io_ptr_, OpenRead()).WillOnce(Return(0));
    ValidReadHeaderExpects();
    EXPECT_CALL(*mock_crypto_ptr_, VerifyPasswordHash(_, _)).WillOnce(Return(0));
    EXPECT_CALL(*mock_crypto_ptr_, DeriveEncryptionKey(_, _, _, _)).WillOnce(Return(-1));
    EXPECT_CALL(*mock_file_io_ptr_, CloseRead()).Times(1);

    unsigned char password[] = "pwd";
    EXPECT_EQ(store_->LoadStore(password), Store::LOAD_STORE_KEY_DERIVATION_ERR);
}

TEST_F(StoreTest, LoadStore_InvalidGetSize_ReturnsDataReadErr) {
    EXPECT_CALL(*mock_crypto_ptr_, HashLen()).WillRepeatedly(Return(hash_len_));
    EXPECT_CALL(*mock_crypto_ptr_, SaltLen()).WillRepeatedly(Return(salt_len_));
    EXPECT_CALL(*mock_crypto_ptr_, EncryptionKeyLen()).WillRepeatedly(Return(encryption_key_len_));

    EXPECT_CALL(*mock_file_io_ptr_, OpenRead()).WillOnce(Return(0));
    ValidReadHeaderExpects();
    EXPECT_CALL(*mock_crypto_ptr_, VerifyPasswordHash(_, _)).WillOnce(Return(0));
    EXPECT_CALL(*mock_crypto_ptr_, DeriveEncryptionKey(_, _, _, _)).WillOnce(Return(0));
    EXPECT_CALL(*mock_crypto_ptr_, Memzero(_, _)).Times(1);

    EXPECT_CALL(*mock_file_io_ptr_, GetSize(_)).WillOnce(Return(0));
    EXPECT_CALL(*mock_file_io_ptr_, CloseRead()).Times(1);

    unsigned char password[] = "pwd";
    EXPECT_EQ(store_->LoadStore(password), Store::LOAD_STORE_DATA_READ_ERR);
}

TEST_F(StoreTest, LoadStore_ReadDataFails_ReturnsDataDecryptErr) {
    EXPECT_CALL(*mock_crypto_ptr_, HashLen()).WillRepeatedly(Return(hash_len_));
    EXPECT_CALL(*mock_crypto_ptr_, SaltLen()).WillRepeatedly(Return(salt_len_));
    EXPECT_CALL(*mock_crypto_ptr_, EncryptionKeyLen()).WillRepeatedly(Return(encryption_key_len_));

    EXPECT_CALL(*mock_file_io_ptr_, OpenRead()).WillOnce(Return(0));
    EXPECT_CALL(*mock_file_io_ptr_, GetPositionRead()).WillOnce(Return(0)).WillOnce(Return(hash_len_ + salt_len_));
    EXPECT_CALL(*mock_file_io_ptr_, Read(_, _))
        .WillOnce(Return(true))
        .WillOnce(Return(true))
        .WillOnce(Return(true))
        .WillOnce(Return(false)); // Invalid read for ReadData
    EXPECT_CALL(*mock_crypto_ptr_, VerifyPasswordHash(_, _)).WillOnce(Return(0));
    EXPECT_CALL(*mock_crypto_ptr_, DeriveEncryptionKey(_, _, _, _)).WillOnce(Return(0));
    EXPECT_CALL(*mock_crypto_ptr_, Memzero(_, _)).Times(2);

    uintmax_t store_data_size = 64;
    EXPECT_CALL(*mock_file_io_ptr_, GetSize(_)).WillOnce(Return(store_data_size + hash_len_ + salt_len_));
    EXPECT_CALL(*mock_file_io_ptr_, CloseRead()).Times(1);

    unsigned char password[] = "pwd";
    EXPECT_EQ(store_->LoadStore(password), Store::LOAD_STORE_DATA_DECRYPT_ERR);
}

// SaveStore
TEST_F(StoreTest, SaveStore_NoData_ReturnsValid) {
    EXPECT_EQ(store_->SaveStore(), Store::SAVE_STORE_VALID);
}

TEST_F(StoreTest, SaveStore_Data_ReturnsValid) {
    CreditCard card;
    store_->AddCard(card);

    EXPECT_CALL(*mock_file_io_ptr_, OpenWriteTemp()).WillOnce(Return(0));

    EXPECT_CALL(*mock_crypto_ptr_, HashLen()).WillRepeatedly(Return(hash_len_));
    EXPECT_CALL(*mock_crypto_ptr_, SaltLen()).WillRepeatedly(Return(salt_len_));
    EXPECT_CALL(*mock_file_io_ptr_, GetPositionWriteTemp())
        .WillOnce(Return(0))
        .WillOnce(Return(hash_len_ + salt_len_))
        .WillOnce(
            Return(hash_len_ + salt_len_ + encryption_header_len_ + TestGetCardsSize() + encryption_added_bytes_));
    EXPECT_CALL(*mock_file_io_ptr_, WriteTemp(_, _))
        .WillOnce(Return(0))
        .WillOnce(Return(0))
        .WillOnce(Return(0))
        .WillOnce(Return(0));
    EXPECT_CALL(*mock_crypto_ptr_, EncryptionHeaderLen()).WillOnce(Return(encryption_header_len_));
    EXPECT_CALL(*mock_crypto_ptr_, EncryptionAddedBytes()).WillOnce(Return(encryption_header_len_));
    EXPECT_CALL(*mock_crypto_ptr_, EncryptBuf(_, _, _, _, _)).WillOnce(Return(0));

    EXPECT_CALL(*mock_crypto_ptr_, Memzero(_, _)).Times(1);
    EXPECT_CALL(*mock_file_io_ptr_, CloseWriteTemp()).Times(1);
    EXPECT_CALL(*mock_file_io_ptr_, CommitTemp()).WillOnce(Return(0));

    EXPECT_EQ(store_->SaveStore(), Store::SAVE_STORE_VALID);
}

TEST_F(StoreTest, SaveStore_OpenWriteTempFails_ReturnsOpenErr) {
    CreditCard card;
    store_->AddCard(card);

    EXPECT_CALL(*mock_file_io_ptr_, OpenWriteTemp()).WillOnce(Return(-1));
    EXPECT_EQ(Store::SAVE_STORE_OPEN_ERR, store_->SaveStore());
}

TEST_F(StoreTest, SaveStore_WriteHeaderFails_ReturnsHeaderErr) {
    CreditCard card;
    store_->AddCard(card);

    EXPECT_CALL(*mock_file_io_ptr_, OpenWriteTemp()).WillOnce(Return(0));
    EXPECT_CALL(*mock_file_io_ptr_, GetPositionWriteTemp())
        .WillOnce(Return(-1)); // Invalid initial position for WriteHeader
    EXPECT_CALL(*mock_file_io_ptr_, CloseWriteTemp()).Times(1);

    EXPECT_EQ(store_->SaveStore(), Store::SAVE_STORE_HEADER_ERR);
}

TEST_F(StoreTest, SaveStore_WriteDataFails_ReturnsWriteDataErr) {
    CreditCard card;
    store_->AddCard(card);

    EXPECT_CALL(*mock_file_io_ptr_, OpenWriteTemp()).WillOnce(Return(0));
    ValidWriteHeaderExpects();

    // EncryptBuf call fails in WriteData
    EXPECT_CALL(*mock_crypto_ptr_, EncryptionHeaderLen()).WillOnce(Return(encryption_header_len_));
    EXPECT_CALL(*mock_crypto_ptr_, EncryptionAddedBytes()).WillOnce(Return(encryption_header_len_));
    EXPECT_CALL(*mock_crypto_ptr_, EncryptBuf(_, _, _, _, _)).WillOnce(Return(-1));

    EXPECT_CALL(*mock_file_io_ptr_, CloseWriteTemp()).Times(1);

    EXPECT_EQ(store_->SaveStore(), Store::SAVE_STORE_WRITE_DATA_ERR);
}

TEST_F(StoreTest, SaveStore_CommitTempFails_ReturnsCommitTempErr) {
    CreditCard card;
    store_->AddCard(card);

    EXPECT_CALL(*mock_file_io_ptr_, OpenWriteTemp()).WillOnce(Return(0));

    EXPECT_CALL(*mock_crypto_ptr_, HashLen()).WillRepeatedly(Return(hash_len_));
    EXPECT_CALL(*mock_crypto_ptr_, SaltLen()).WillRepeatedly(Return(salt_len_));
    EXPECT_CALL(*mock_file_io_ptr_, GetPositionWriteTemp())
        .WillOnce(Return(0))
        .WillOnce(Return(hash_len_ + salt_len_))
        .WillOnce(
            Return(hash_len_ + salt_len_ + encryption_header_len_ + TestGetCardsSize() + encryption_added_bytes_));
    EXPECT_CALL(*mock_file_io_ptr_, WriteTemp(_, _))
        .WillOnce(Return(0))
        .WillOnce(Return(0))
        .WillOnce(Return(0))
        .WillOnce(Return(0));
    EXPECT_CALL(*mock_crypto_ptr_, EncryptionHeaderLen()).WillOnce(Return(encryption_header_len_));
    EXPECT_CALL(*mock_crypto_ptr_, EncryptionAddedBytes()).WillOnce(Return(encryption_header_len_));
    EXPECT_CALL(*mock_crypto_ptr_, EncryptBuf(_, _, _, _, _)).WillOnce(Return(0));

    EXPECT_CALL(*mock_crypto_ptr_, Memzero(_, _)).Times(1);
    EXPECT_CALL(*mock_file_io_ptr_, CloseWriteTemp()).Times(1);
    EXPECT_CALL(*mock_file_io_ptr_, CommitTemp()).WillOnce(Return(-1));

    EXPECT_EQ(store_->SaveStore(), Store::SAVE_STORE_COMMIT_TEMP_ERR);
}

// AddCard
TEST_F(StoreTest, AddCard_OneCard_ExpectCardsDisplayStringNotEmpty) {
    CreditCard card;
    store_->AddCard(card);
    EXPECT_FALSE(store_->CardsDisplayList().empty());
}

TEST_F(StoreTest, AddCard_TwoCards_ExpectCardsDisplayStringNotEmpty) {
    CreditCard card1;
    store_->AddCard(card1);
    CreditCard card2;
    store_->AddCard(card2);

    EXPECT_FALSE(store_->CardsDisplayList().empty());
}

// DeleteCard
TEST_F(StoreTest, Delete_OneCard_ExpectCardNotInDisplayString) {
    CreditCard card;
    std::string card_name = "Card1";
    card.SetName(card_name);

    store_->AddCard(card);
    store_->DeleteCard(0);

    std::vector<std::pair<uint32_t, std::string>> cards_display_list = store_->CardsDisplayList();
    bool card_found = false;
    for (const auto &card_display : cards_display_list) {
        card_found = card_found || card_display.second == card_name;
    }
    EXPECT_FALSE(card_found);
}

TEST_F(StoreTest, DeleteCard_TwoCards_ExpectOnlyDeletedCardNotInDisplayString) {
    CreditCard card1;
    std::string card1_name = "Card1";
    card1.SetName(card1_name);
    store_->AddCard(card1);

    CreditCard card2;
    std::string card2_name = "Card2";
    card2.SetName(card2_name);
    store_->AddCard(card2);

    store_->DeleteCard(0);

    std::vector<std::pair<uint32_t, std::string>> cards_display_list = store_->CardsDisplayList();
    bool card1_found = false;
    bool card2_found = false;
    for (const auto &card_display : cards_display_list) {
        card1_found = card1_found || card_display.second == card1_name;
        card2_found = card2_found || card_display.second == card2_name;
    }
    EXPECT_FALSE(card1_found);
    EXPECT_TRUE(card2_found);
}

TEST_F(StoreTest, DeleteCard_ThreeCards_ExpectOnlyDeletedCardNotInDisplayString) {
    CreditCard card1;
    std::string card1_name = "Card1";
    card1.SetName(card1_name);
    store_->AddCard(card1);

    CreditCard card2;
    std::string card2_name = "Card2";
    card2.SetName(card2_name);
    store_->AddCard(card2);

    CreditCard card3;
    std::string card3_name = "Card3";
    card3.SetName(card3_name);
    store_->AddCard(card3);

    store_->DeleteCard(1);

    std::vector<std::pair<uint32_t, std::string>> cards_display_list = store_->CardsDisplayList();
    bool card1_found = false;
    bool card2_found = false;
    bool card3_found = false;
    for (const auto &card_display : cards_display_list) {
        card1_found = card1_found || card_display.second == card1_name;
        card2_found = card2_found || card_display.second == card2_name;
        card3_found = card3_found || card_display.second == card3_name;
    }
    EXPECT_TRUE(card1_found);
    EXPECT_FALSE(card2_found);
    EXPECT_TRUE(card3_found);
}

// StoreExists
TEST_F(StoreTest, StoreExists_MainFileExists_ReturnsTrue) {
    EXPECT_CALL(*mock_file_io_ptr_, GetExists(false)).WillOnce(Return(true));
    EXPECT_TRUE(store_->StoreExists(false));
}

TEST_F(StoreTest, StoreExists_TmpFileExists_ReturnsTrue) {
    EXPECT_CALL(*mock_file_io_ptr_, GetExists(true)).WillOnce(Return(true));
    EXPECT_TRUE(store_->StoreExists(true));
}

TEST_F(StoreTest, StoreExists_MainFileNotExists_ReturnsFalse) {
    EXPECT_CALL(*mock_file_io_ptr_, GetExists(false)).WillOnce(Return(false));
    EXPECT_FALSE(store_->StoreExists(false));
}

TEST_F(StoreTest, StoreExists_TmpFileNotExists_ReturnsFalse) {
    EXPECT_CALL(*mock_file_io_ptr_, GetExists(true)).WillOnce(Return(false));
    EXPECT_FALSE(store_->StoreExists(true));
}

// DeleteStore
TEST_F(StoreTest, DeleteStore_MainFile_Returns0) {
    EXPECT_CALL(*mock_file_io_ptr_, Delete(false)).WillOnce(Return(true));
    EXPECT_EQ(store_->DeleteStore(false), 0);
}

TEST_F(StoreTest, DeleteStore_TmpFile_Returns0) {
    EXPECT_CALL(*mock_file_io_ptr_, Delete(true)).WillOnce(Return(true));
    EXPECT_EQ(store_->DeleteStore(true), 0);
}

TEST_F(StoreTest, DeleteStore_MainDeleteFails_ReturnsNegative1) {
    EXPECT_CALL(*mock_file_io_ptr_, Delete(false)).WillOnce(Return(false));
    EXPECT_EQ(store_->DeleteStore(false), -1);
}

TEST_F(StoreTest, DeleteStore_TmpDeleteFails_ReturnsNegative1) {
    EXPECT_CALL(*mock_file_io_ptr_, Delete(true)).WillOnce(Return(false));
    EXPECT_EQ(store_->DeleteStore(true), -1);
}

// CardsDisplayList
TEST_F(StoreTest, CardsDisplayList_NoCards_ReturnsEmpty) { EXPECT_TRUE(store_->CardsDisplayList().empty()); }

TEST_F(StoreTest, CardsDisplayString_OneCard_ReturnsOneCardVector) {
    CreditCard card1;
    card1.SetName("Card1");

    store_->AddCard(card1);

    auto cards_list = store_->CardsDisplayList();
    EXPECT_EQ(cards_list.size(), 1);
    EXPECT_EQ(cards_list[0].second, "Card1");
}

TEST_F(StoreTest, CardsDisplayList_MultipleCards_ReturnsTwoCardVector) {
    CreditCard card1;
    card1.SetName("Card1");
    CreditCard card2;
    card2.SetName("Card2");

    store_->AddCard(card1);
    store_->AddCard(card2);

    auto cards_list = store_->CardsDisplayList();
    EXPECT_EQ(cards_list.size(), 2);
    EXPECT_EQ(cards_list[0].second, "Card1");
    EXPECT_EQ(cards_list[1].second, "Card2");
}

// GetCardById
TEST_F(StoreTest, GetCardById_OnlyCard_ReturnsCorrectCard) {
    CreditCard card1;
    card1.SetName("Card1");

    store_->AddCard(card1);
    auto card = store_->GetCardById(0);

    EXPECT_EQ(card.FormatText(), card1.FormatText());
}

TEST_F(StoreTest, GetCardById_SelectSecondFromThreeCards_ReturnsCorrectCard) {
    CreditCard card1;
    card1.SetName("Card1");
    CreditCard card2;
    card1.SetName("Card2");
    CreditCard card3;
    card1.SetName("Card3");

    store_->AddCard(card1);
    store_->AddCard(card2);
    store_->AddCard(card3);
    auto card = store_->GetCardById(1);

    EXPECT_EQ(card.FormatText(), card2.FormatText());
}

// ReadHeader
TEST_F(StoreTest, ReadHeader_Valid_Returns0) {
    ValidReadHeaderExpects();

    unsigned char hash[hash_len_];
    unsigned char salt[salt_len_];
    EXPECT_EQ(TestReadHeader(hash, salt), 0);
}

TEST_F(StoreTest, ReadHeader_InvalidInitialReadPos_ReturnsNegative1) {
    EXPECT_CALL(*mock_file_io_ptr_, GetPositionRead()).WillOnce(Return(-1));

    unsigned char hash[hash_len_];
    unsigned char salt[salt_len_];
    EXPECT_EQ(TestReadHeader(hash, salt), -1);
}

TEST_F(StoreTest, ReadHeader_HashReadFails_ReturnsNegative1) {
    EXPECT_CALL(*mock_file_io_ptr_, GetPositionRead()).WillOnce(Return(0));
    EXPECT_CALL(*mock_crypto_ptr_, HashLen()).WillOnce(Return(hash_len_));
    EXPECT_CALL(*mock_file_io_ptr_, Read(_, _)).WillOnce(Return(false));

    unsigned char hash[hash_len_];
    unsigned char salt[salt_len_];
    EXPECT_EQ(TestReadHeader(hash, salt), -1);
}

TEST_F(StoreTest, ReadHeader_SaltReadFails_ReturnsNegative1) {
    EXPECT_CALL(*mock_file_io_ptr_, GetPositionRead()).WillOnce(Return(0));
    EXPECT_CALL(*mock_crypto_ptr_, HashLen()).WillOnce(Return(hash_len_));
    EXPECT_CALL(*mock_crypto_ptr_, SaltLen()).WillOnce(Return(salt_len_));
    EXPECT_CALL(*mock_file_io_ptr_, Read(_, _)).WillOnce(Return(true)).WillOnce(Return(false));

    unsigned char hash[hash_len_];
    unsigned char salt[salt_len_];
    EXPECT_EQ(TestReadHeader(hash, salt), -1);
}

TEST_F(StoreTest, ReadHeader_InvalidFinalReadPos_ReturnsNegative1) {
    EXPECT_CALL(*mock_file_io_ptr_, GetPositionRead()).WillOnce(Return(0)).WillOnce(Return(-1));
    EXPECT_CALL(*mock_file_io_ptr_, Read(_, _)).WillOnce(Return(true)).WillOnce(Return(true));
    EXPECT_CALL(*mock_crypto_ptr_, HashLen()).WillRepeatedly(Return(hash_len_));
    EXPECT_CALL(*mock_crypto_ptr_, SaltLen()).WillRepeatedly(Return(salt_len_));

    unsigned char hash[hash_len_];
    unsigned char salt[salt_len_];
    EXPECT_EQ(TestReadHeader(hash, salt), -1);
}

// ReadData
TEST_F(StoreTest, ReadData_DataSize0_Returns0) {
    ValidReadDataExpects();

    uintmax_t data_size = 0;
    unsigned char decrypted_data[data_size];
    uint64_t dec_data_size;
    EXPECT_EQ(TestReadData(decrypted_data, data_size, &dec_data_size), 0);
}

TEST_F(StoreTest, ReadData_LargeDataSize_Returns0) {
    ValidReadDataExpects();

    uintmax_t data_size = 1000000;
    unsigned char decrypted_data[data_size];
    uint64_t dec_data_size;
    EXPECT_EQ(TestReadData(decrypted_data, data_size, &dec_data_size), 0);
}

TEST_F(StoreTest, ReadData_EncryptionHeaderReadFails_ReturnsNegative1) {
    EXPECT_CALL(*mock_crypto_ptr_, EncryptionHeaderLen()).WillRepeatedly(Return(hash_len_));
    EXPECT_CALL(*mock_file_io_ptr_, Read(_, _)).WillOnce(Return(false));

    uintmax_t data_size = 16;
    unsigned char decrypted_data[data_size];
    uint64_t dec_data_size;
    EXPECT_EQ(TestReadData(decrypted_data, data_size, &dec_data_size), -1);
}

TEST_F(StoreTest, ReadData_EncryptedDataReadFails_ReturnsNegative1) {
    EXPECT_CALL(*mock_crypto_ptr_, EncryptionHeaderLen()).WillRepeatedly(Return(hash_len_));
    EXPECT_CALL(*mock_file_io_ptr_, Read(_, _)).WillOnce(Return(true)).WillOnce(Return(false));

    uintmax_t data_size = 16;
    unsigned char decrypted_data[data_size];
    uint64_t dec_data_size;
    EXPECT_EQ(TestReadData(decrypted_data, data_size, &dec_data_size), -1);
}

TEST_F(StoreTest, ReadData_DecryptBufFails_ReturnsNegative1) {
    EXPECT_CALL(*mock_crypto_ptr_, EncryptionHeaderLen()).WillRepeatedly(Return(hash_len_));
    EXPECT_CALL(*mock_file_io_ptr_, Read(_, _)).WillOnce(Return(true)).WillOnce(Return(true));
    EXPECT_CALL(*mock_crypto_ptr_, DecryptBuf(_, _, _, _, _, _)).WillOnce(Return(-1));

    uintmax_t data_size = 16;
    unsigned char decrypted_data[data_size];
    uint64_t dec_data_size;
    EXPECT_EQ(TestReadData(decrypted_data, data_size, &dec_data_size), -1);
}

// WriteHeader
TEST_F(StoreTest, WriteHeader_NoData_Returns0) {
    ValidWriteHeaderExpects();

    const unsigned char *hash;
    const unsigned char *salt;
    EXPECT_EQ(TestWriteHeader(hash, salt), 0);
}

TEST_F(StoreTest, WriteHeader_TestData_Returns0) {
    ValidWriteHeaderExpects();

    const auto *hash = reinterpret_cast<const unsigned char *>("test");
    const auto *salt = reinterpret_cast<const unsigned char *>("test");
    EXPECT_EQ(TestWriteHeader(hash, salt), 0);
}

TEST_F(StoreTest, WriteHeader_InvalidInitialWritePos_ReturnsNegative1) {
    EXPECT_CALL(*mock_file_io_ptr_, GetPositionWriteTemp()).WillOnce(Return(-1));

    const auto *hash = reinterpret_cast<const unsigned char *>("test");
    const auto *salt = reinterpret_cast<const unsigned char *>("test");
    EXPECT_EQ(TestWriteHeader(hash, salt), -1);
}

TEST_F(StoreTest, WriteHeader_InvalidFinalWritePos_Returns0) {
    EXPECT_CALL(*mock_file_io_ptr_, GetPositionWriteTemp()).WillOnce(Return(0)).WillOnce(Return(-1));

    EXPECT_CALL(*mock_file_io_ptr_, WriteTemp(_, _)).WillOnce(Return(0)).WillOnce(Return(0));
    EXPECT_CALL(*mock_crypto_ptr_, HashLen()).WillRepeatedly(Return(hash_len_));
    EXPECT_CALL(*mock_crypto_ptr_, SaltLen()).WillRepeatedly(Return(salt_len_));

    const auto *hash = reinterpret_cast<const unsigned char *>("test");
    const auto *salt = reinterpret_cast<const unsigned char *>("test");
    EXPECT_EQ(TestWriteHeader(hash, salt), -1);
}

// WriteData
TEST_F(StoreTest, WriteData_NoData_Returns0) {
    unsigned char dec_data[0];
    uintmax_t dec_data_size = 0;

    ValidWriteDataExpects(dec_data_size);

    EXPECT_EQ(TestWriteData(dec_data, dec_data_size), 0);
}

TEST_F(StoreTest, WriteData_TestData_Returns0) {
    uint64_t dec_data_size = 10;
    unsigned char dec_data[dec_data_size];

    ValidWriteDataExpects(dec_data_size);

    EXPECT_EQ(TestWriteData(dec_data, dec_data_size), 0);
}

TEST_F(StoreTest, WriteData_EncryptionFails_ReturnsNegative1) {
    uint64_t dec_data_size = 10;
    unsigned char dec_data[dec_data_size];

    EXPECT_CALL(*mock_crypto_ptr_, EncryptionHeaderLen()).WillOnce(Return(encryption_header_len_));
    EXPECT_CALL(*mock_crypto_ptr_, EncryptionAddedBytes()).WillOnce(Return(encryption_header_len_));
    EXPECT_CALL(*mock_crypto_ptr_, EncryptBuf(_, _, _, _, _)).WillOnce(Return(-1));

    EXPECT_EQ(TestWriteData(dec_data, dec_data_size), -1);
}

TEST_F(StoreTest, WriteData_InvalidWriteTempPos_ReturnsNegative1) {
    uint64_t dec_data_size = 10;
    unsigned char dec_data[dec_data_size];

    EXPECT_CALL(*mock_crypto_ptr_, EncryptionHeaderLen()).WillOnce(Return(encryption_header_len_));
    EXPECT_CALL(*mock_crypto_ptr_, EncryptionAddedBytes()).WillOnce(Return(encryption_header_len_));
    EXPECT_CALL(*mock_crypto_ptr_, EncryptBuf(_, _, _, _, _)).WillOnce(Return(0));

    EXPECT_CALL(*mock_file_io_ptr_, WriteTemp(_, _)).WillOnce(Return(0)).WillOnce(Return(0));

    EXPECT_CALL(*mock_crypto_ptr_, HashLen()).WillRepeatedly(Return(hash_len_));
    EXPECT_CALL(*mock_crypto_ptr_, SaltLen()).WillRepeatedly(Return(salt_len_));
    EXPECT_CALL(*mock_file_io_ptr_, GetPositionWriteTemp()).WillOnce(Return(-1));

    EXPECT_EQ(TestWriteData(dec_data, dec_data_size), -1);
}

// GetCardsSize
TEST_F(StoreTest, GetCardsSize_NoCards_Returns0) { EXPECT_EQ(TestGetCardsSize(), 0); }

TEST_F(StoreTest, GetCardsSize_OneCard_ReturnsLoadedStringSize) {
    TestLoadCards(const_cast<unsigned char *>(reinterpret_cast<const unsigned char *>(one_card_formatted_.c_str())));
    EXPECT_EQ(TestGetCardsSize(), one_card_formatted_.size());
}

TEST_F(StoreTest, GetCardsSize_TwoCards_ReturnsLoadedStringSize) {
    TestLoadCards(const_cast<unsigned char *>(reinterpret_cast<const unsigned char *>(two_cards_formatted_.c_str())));
    EXPECT_EQ(TestGetCardsSize(), two_cards_formatted_.size());
}

// CardsFormatted
TEST_F(StoreTest, CardsFormatted_NoCards_Returns0) {
    unsigned char cards[10];
    cards[0] = 0;

    EXPECT_EQ(TestCardsFormatted(cards), 0);
}

TEST_F(StoreTest, CardsFormatted_OneCard_ReturnsCardsSizeAndLoadedString) {
    std::string card_formatted_cpy = one_card_formatted_;
    unsigned char cards[card_formatted_cpy.size()];
    TestLoadCards(const_cast<unsigned char *>(reinterpret_cast<const unsigned char *>(card_formatted_cpy.c_str())));

    EXPECT_EQ(TestCardsFormatted(cards), one_card_formatted_.size());
    EXPECT_EQ(memcmp(cards, one_card_formatted_.c_str(), one_card_formatted_.size()), 0);
}

TEST_F(StoreTest, CardsFormatted_TwoCards_ReturnsCardsSizeAndLoadedString) {
    std::string cards_formatted_cpy = two_cards_formatted_;
    unsigned char cards[cards_formatted_cpy.size()];
    TestLoadCards(const_cast<unsigned char *>(reinterpret_cast<const unsigned char *>(cards_formatted_cpy.c_str())));

    EXPECT_EQ(TestCardsFormatted(cards), two_cards_formatted_.size());
    EXPECT_EQ(memcmp(cards, two_cards_formatted_.c_str(), two_cards_formatted_.size()), 0);
}

// LoadCards
TEST_F(StoreTest, LoadCards_EmptyData_ExpectCardsSize0) {
    TestLoadCards(nullptr);
    EXPECT_EQ(TestGetCardsSize(), 0);
}

TEST_F(StoreTest, LoadCards_OneCard_ExpectOneCardsSize) {
    TestLoadCards(const_cast<unsigned char *>(reinterpret_cast<const unsigned char *>(one_card_formatted_.c_str())));
    EXPECT_EQ(TestGetCardsSize(), one_card_formatted_.size());
}

TEST_F(StoreTest, LoadCards_OneCard_ExpectTwoCardsSize) {
    TestLoadCards(const_cast<unsigned char *>(reinterpret_cast<const unsigned char *>(two_cards_formatted_.c_str())));
    EXPECT_EQ(TestGetCardsSize(), two_cards_formatted_.size());
}
