#ifndef ICRYPTO_HPP
#define ICRYPTO_HPP

#include <cstdint>
#include <cstdlib>
#include <string>

class ICrypto {
  public:
    virtual auto InitCrypto() -> int = 0;

    virtual auto EncryptionAddedBytes() const -> uint64_t = 0;
    virtual auto EncryptionHeaderLen() const -> uint64_t = 0;
    virtual auto EncryptionKeyLen() const -> uint64_t = 0;
    virtual auto HashLen() const -> uint64_t = 0;
    virtual auto SaltLen() const -> uint64_t = 0;

    virtual auto DeriveEncryptionKey(unsigned char *key, size_t key_len, const unsigned char *password,
                                     const unsigned char *salt) -> int = 0;
    virtual auto EncryptBuf(unsigned char *out_data, unsigned char *header, const unsigned char *buf, uintmax_t buf_len,
                            const unsigned char *key) -> int = 0;
    virtual auto HashPassword(unsigned char *hash, const unsigned char *password) -> int = 0;
    virtual void GenerateSalt(unsigned char *salt) = 0;

    virtual auto DecryptBuf(unsigned char *out_data, uint64_t *out_len, unsigned char *header,
                            unsigned char *encrypted_buf, uintmax_t buf_len, const unsigned char *key) -> int = 0;
    virtual auto VerifyPasswordHash(const unsigned char *hash, const unsigned char *password) -> int = 0;

    virtual void Memzero(void *ptr, size_t len) = 0;
};

#endif // ICRYPTO_HPP
