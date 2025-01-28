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

int encrypt_buf(unsigned char *out_data, unsigned char *header, const unsigned char *buf, uintmax_t buf_len,
                const unsigned char *key) {
    if (buf_len > crypto_secretstream_xchacha20poly1305_MESSAGEBYTES_MAX) {
        return -1;
    }

    crypto_secretstream_xchacha20poly1305_state state;
    crypto_secretstream_xchacha20poly1305_init_push(&state, header, key);
    unsigned long long out_len = 0;
    crypto_secretstream_xchacha20poly1305_push(&state, out_data, &out_len, buf, buf_len, nullptr, 0,
                                               crypto_secretstream_xchacha20poly1305_TAG_FINAL);
    if (out_len != buf_len + crypto_secretstream_xchacha20poly1305_ABYTES) {
        return -1;
    }

    return 0;
}

int hash_password(unsigned char *hash, const unsigned char *password) {
    int password_len = strlen((char *)password);
    if (password_len < crypto_pwhash_PASSWD_MIN || password_len > crypto_pwhash_PASSWD_MAX) {
        return -1;
    }

    return crypto_pwhash_str((char *)hash, (const char *)password, password_len, OPS_LIMIT, MEM_LIMIT);
}

int decrypt_buf(unsigned char *out_data, unsigned long long *out_len, unsigned char *header,
                unsigned char *encrypted_buf, uintmax_t buf_len, const unsigned char *key) {
    crypto_secretstream_xchacha20poly1305_state state;
    if (crypto_secretstream_xchacha20poly1305_init_pull(&state, header, key) != 0) {
        return -1;
    }

    unsigned char tag;
    if (crypto_secretstream_xchacha20poly1305_pull(&state, out_data, out_len, &tag, encrypted_buf, buf_len, nullptr,
                                                   0) != 0) {
        return -1;
    }
    if (tag != crypto_secretstream_xchacha20poly1305_TAG_FINAL) {
        return -1;
    }

    return 0;
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
