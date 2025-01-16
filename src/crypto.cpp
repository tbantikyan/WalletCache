#include "crypto.hpp"

#include <string.h>

int init_crypto() { return sodium_init(); }

int derive_encryption_key(unsigned char *key, size_t key_len, const unsigned char *password,
                          const unsigned char *salt) {
    int password_len = strlen((char *)password);
    if (password_len < crypto_pwhash_PASSWD_MIN || password_len > crypto_pwhash_PASSWD_MAX) {
        return -1;
    }

    return crypto_pwhash(key, key_len, (const char *)password, password_len, salt, OPS_LIMIT, MEM_LIMIT, HASH_ALG);
}

int hash_password(unsigned char *hash, const unsigned char *password) {
    int password_len = strlen((char *)password);
    if (password_len < crypto_pwhash_PASSWD_MIN || password_len > crypto_pwhash_PASSWD_MAX) {
        return -1;
    }

    return crypto_pwhash_str((char *)hash, (const char *)password, password_len, OPS_LIMIT, MEM_LIMIT);
}

int verify_password_hash(unsigned char *hash, const unsigned char *password) {
    int password_len = strlen((char *)password);
    if (password_len < crypto_pwhash_PASSWD_MIN || password_len > crypto_pwhash_PASSWD_MAX) {
        return -1;
    }

    return crypto_pwhash_str_verify((const char *)hash, (const char *)password, password_len);
}

void generate_salt(unsigned char *salt) { randombytes_buf((char *)salt, SALT_LEN); }

void memzero(void *const ptr, const size_t len) { sodium_memzero(ptr, len); }

void secure_cpy_str_to_buf(std::string &str, unsigned char *buf) {
    memcpy(buf, str.c_str(), str.size());
    buf[str.size()] = '\0';
}
