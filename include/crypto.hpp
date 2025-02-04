#ifndef CRYPTO_HPP
#define CRYPTO_HPP

#include <_types/_uintmax_t.h>
#include <cstdlib>
#include <sodium.h>
#include <string>

#define ENCRYPTION_ADDED_BYTES (crypto_secretstream_xchacha20poly1305_ABYTES)
#define ENCRYPTION_HEADER_LEN (crypto_secretstream_xchacha20poly1305_HEADERBYTES)
#define ENCYRPTION_KEY_LEN (crypto_secretstream_xchacha20poly1305_KEYBYTES)
#define HASH_ALG (crypto_pwhash_ALG_ARGON2ID13)
#define HASH_LEN (crypto_pwhash_STRBYTES)
#define OPS_LIMIT (crypto_pwhash_OPSLIMIT_SENSITIVE)
#define MEM_LIMIT (crypto_pwhash_MEMLIMIT_SENSITIVE)
#define SALT_LEN (crypto_pwhash_SALTBYTES)

auto InitCrypto() -> int;

auto DeriveEncryptionKey(unsigned char *key, size_t key_len, const unsigned char *password, const unsigned char *salt)
    -> int;
auto EncryptBuf(unsigned char *out_data, unsigned char *header, const unsigned char *buf, uintmax_t buf_len,
                const unsigned char *key) -> int;
auto HashPassword(unsigned char *hash, const unsigned char *password) -> int;
void GenerateSalt(unsigned char *salt);

auto DecryptBuf(unsigned char *out_data, uint64_t *out_len, unsigned char *header, unsigned char *encrypted_buf,
                uintmax_t buf_len, const unsigned char *key) -> int;
auto VerifyPasswordHash(const unsigned char *hash, const unsigned char *password) -> int;

void Memzero(void *ptr, size_t len);
void SecureCpyStrToBuf(std::string &str, unsigned char *buf);

#endif // CRYPTO_HPP
