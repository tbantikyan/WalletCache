#ifndef CRYPTO_HPP
#define CRYPTO_HPP

#include <_types/_uintmax_t.h>
#include <sodium.h>
#include <stdlib.h>
#include <string>

#define ENCRYPTION_ADDED_BYTES (crypto_secretstream_xchacha20poly1305_ABYTES)
#define ENCRYPTION_HEADER_LEN (crypto_secretstream_xchacha20poly1305_HEADERBYTES)
#define ENCYRPTION_KEY_LEN (crypto_secretstream_xchacha20poly1305_KEYBYTES)
#define HASH_ALG (crypto_pwhash_ALG_ARGON2ID13)
#define HASH_LEN (crypto_pwhash_STRBYTES)
#define OPS_LIMIT (crypto_pwhash_OPSLIMIT_SENSITIVE)
#define MEM_LIMIT (crypto_pwhash_MEMLIMIT_SENSITIVE)
#define SALT_LEN (crypto_pwhash_SALTBYTES)

int init_crypto();

int derive_encryption_key(unsigned char *key, size_t key_len, const unsigned char *password, const unsigned char *salt);
int encrypt_buf(unsigned char *out_data, unsigned char *header, const unsigned char *buf, uintmax_t buf_len,
                const unsigned char *key);
int hash_password(unsigned char *hash, const unsigned char *password);
void generate_salt(unsigned char *salt);

int decrypt_buf(unsigned char *out_data, unsigned long long *out_len, unsigned char *header,
                unsigned char *encrypted_buf, uintmax_t buf_len, const unsigned char *key);
int verify_password_hash(unsigned char *hash, const unsigned char *password);

void memzero(void *const ptr, const size_t len);
void secure_cpy_str_to_buf(std::string &str, unsigned char *buf);

#endif // CRYPTO_HPP
