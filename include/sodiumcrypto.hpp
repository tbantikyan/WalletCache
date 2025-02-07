#ifndef SODIUMCRYPTO_HPP
#define SODIUMCRYPTO_HPP

#include "icrypto.hpp"

#include <sodium.h>

class SodiumCrypto : public ICrypto {
  public:
    auto InitCrypto() -> int override;

    auto EncryptionAddedBytes() const -> uint64_t override;
    auto EncryptionHeaderLen() const -> uint64_t override;
    auto EncryptionKeyLen() const -> uint64_t override;
    auto HashLen() const -> uint64_t override;
    auto SaltLen() const -> uint64_t override;

    auto DeriveEncryptionKey(unsigned char *key, size_t key_len, const unsigned char *password,
                             const unsigned char *salt) -> int override;

    auto EncryptBuf(unsigned char *out_data, unsigned char *header, const unsigned char *buf, uintmax_t buf_len,
                    const unsigned char *key) -> int override;

    auto HashPassword(unsigned char *hash, const unsigned char *password) -> int override;

    void GenerateSalt(unsigned char *salt) override;

    auto DecryptBuf(unsigned char *out_data, uint64_t *out_len, unsigned char *header, unsigned char *encrypted_buf,
                    uintmax_t buf_len, const unsigned char *key) -> int override;

    auto VerifyPasswordHash(const unsigned char *hash, const unsigned char *password) -> int override;

    void Memzero(void *ptr, size_t len) override;

    void SecureCpyStrToBuf(std::string &str, unsigned char *buf) override;

  private:
    static const uint64_t ENCRYPTION_ADDED_BYTES = crypto_secretstream_xchacha20poly1305_ABYTES;
    static const uint64_t ENCRYPTION_HEADER_LEN = crypto_secretstream_xchacha20poly1305_HEADERBYTES;
    static const uint64_t ENCRYPTION_KEY_LEN = crypto_secretstream_xchacha20poly1305_KEYBYTES;
    static const uint64_t HASH_LEN = crypto_pwhash_STRBYTES;
    static const uint64_t SALT_LEN = crypto_pwhash_SALTBYTES;

    static const uint64_t HASH_ALG = crypto_pwhash_ALG_ARGON2ID13;
    static const uint64_t OPS_LIMIT = crypto_pwhash_OPSLIMIT_SENSITIVE;
    static const uint64_t MEM_LIMIT = crypto_pwhash_MEMLIMIT_SENSITIVE;
};

#endif // SODIUMCRYPTO_HPP
