#include "sodiumcrypto.hpp"

#include <gtest/gtest.h>

class SodiumCryptoTest : public ::testing::Test {
  protected:
    SodiumCrypto crypto_;

    void SetUp() override { ASSERT_GE(sodium_init(), 0); }
};

// InitCrypto
TEST_F(SodiumCryptoTest, InitCrypto_WhenAlreadyInitialized_ReturnsOne) { EXPECT_EQ(crypto_.InitCrypto(), 1); }

// EncryptionAddedBytes
TEST_F(SodiumCryptoTest, EncryptionAddedBytes_ReturnsLibsodiumConstant) {
    EXPECT_EQ(crypto_.EncryptionAddedBytes(), crypto_secretstream_xchacha20poly1305_ABYTES);
}

// EncryptionHeaderLen
TEST_F(SodiumCryptoTest, EncryptionHeaderLen_ReturnsLibsodiumConstant) {
    EXPECT_EQ(crypto_.EncryptionHeaderLen(), crypto_secretstream_xchacha20poly1305_HEADERBYTES);
}

// EncryptionKeyLen
TEST_F(SodiumCryptoTest, EncryptionKeyLen_ReturnsLibsodiumConstant) {
    EXPECT_EQ(crypto_.EncryptionKeyLen(), crypto_secretstream_xchacha20poly1305_KEYBYTES);
}

// HashLen
TEST_F(SodiumCryptoTest, HashLen_ReturnsLibsodiumConstant) { EXPECT_EQ(crypto_.HashLen(), crypto_pwhash_STRBYTES); }

// SaltLen
TEST_F(SodiumCryptoTest, SaltLen_ReturnsLibsodiumConstant) { EXPECT_EQ(crypto_.SaltLen(), crypto_pwhash_SALTBYTES); }

// DeriveEncryptionKey
TEST_F(SodiumCryptoTest, DeriveEncryptionKey_ValidInput_Returns0) {
    unsigned char key[crypto_.EncryptionKeyLen()];
    unsigned char salt[crypto_.SaltLen()];
    const char *password = "valid_password";

    crypto_.GenerateSalt(salt);
    EXPECT_EQ(crypto_.DeriveEncryptionKey(key, crypto_.EncryptionKeyLen(),
                                          reinterpret_cast<const unsigned char *>(password), salt),
              0);
}

// EncryptBuf
TEST_F(SodiumCryptoTest, EncryptBuf_InvalidBufLen_ReturnsNegative1) {
    const std::string plaintext = "Test secret message";
    uint64_t data_len = plaintext.size() + crypto_.EncryptionAddedBytes();
    unsigned char encrypted_data[data_len];
    unsigned char header[crypto_.EncryptionHeaderLen()];
    unsigned char key[crypto_.EncryptionKeyLen()];

    ASSERT_EQ(crypto_.EncryptBuf(encrypted_data, header, reinterpret_cast<const unsigned char *>(plaintext.c_str()),
                                 crypto_secretstream_xchacha20poly1305_MESSAGEBYTES_MAX + 1, key),
              -1);
}

// DecryptBuf
TEST_F(SodiumCryptoTest, EncryptDecryptBuf_SimpleString_ReturnsSameString) {
    const std::string plaintext = "Test secret message";
    uint64_t data_len = plaintext.size() + crypto_.EncryptionAddedBytes();
    unsigned char encrypted[data_len];
    unsigned char header[crypto_.EncryptionHeaderLen()];
    unsigned char key[crypto_.EncryptionKeyLen()];

    ASSERT_EQ(crypto_.EncryptBuf(encrypted, header, reinterpret_cast<const unsigned char *>(plaintext.c_str()),
                                 plaintext.size(), key),
              0);

    unsigned char decrypted[data_len];
    uint64_t decrypted_len;
    ASSERT_EQ(crypto_.DecryptBuf(decrypted, &decrypted_len, header, encrypted, data_len, key), 0);
    decrypted[decrypted_len] = 0;

    ASSERT_EQ(memcmp(decrypted, plaintext.c_str(), plaintext.size()), 0);
}

// HashPassword + VerifyPasswordHash
TEST_F(SodiumCryptoTest, HashPassword_ValidPassword_VerifiesSuccessfully) {
    unsigned char hash[crypto_pwhash_STRBYTES];
    const char *password = "SecurePassword123!";

    ASSERT_EQ(crypto_.HashPassword(hash, reinterpret_cast<const unsigned char *>(password)), 0);
    EXPECT_EQ(crypto_.VerifyPasswordHash(hash, reinterpret_cast<const unsigned char *>(password)), 0);
}

TEST_F(SodiumCryptoTest, VerifyPasswordHash_WrongPassword_ReturnsError) {
    unsigned char hash[crypto_pwhash_STRBYTES];
    const char *password = "password";
    const char *wrong_password = "passw0rd";

    crypto_.HashPassword(hash, reinterpret_cast<const unsigned char *>(password));
    EXPECT_EQ(crypto_.VerifyPasswordHash(hash, reinterpret_cast<const unsigned char *>(wrong_password)), -1);
}

// GenerateSalt 
TEST_F(SodiumCryptoTest, GenerateSalt_ProducesUniqueValues) {
    unsigned char salt1[crypto_.SaltLen()];
    unsigned char salt2[crypto_.SaltLen()];

    crypto_.GenerateSalt(salt1);
    crypto_.GenerateSalt(salt2);

    EXPECT_NE(memcmp(salt1, salt2, crypto_.SaltLen()), 0);
}

// Memzero 
TEST_F(SodiumCryptoTest, Memzero_ZeroesMemory) {
    unsigned char buffer[crypto_.SaltLen()];
    crypto_.GenerateSalt(buffer);

    crypto_.Memzero(buffer, sizeof(buffer));
    for (unsigned char i : buffer) {
        EXPECT_EQ(i, 0);
    }
}
