#include "crypto.hpp"

#include <cstring>

auto InitCrypto() -> int { return sodium_init(); }

auto DeriveEncryptionKey(unsigned char *key, size_t key_len, const unsigned char *password, const unsigned char *salt)
    -> int {
    int password_len = strlen(const_cast<char *>(reinterpret_cast<const char *>(password)));
    if (password_len < crypto_pwhash_PASSWD_MIN || password_len > crypto_pwhash_PASSWD_MAX) {
        return -1;
    }

    return crypto_pwhash(key, key_len, reinterpret_cast<const char *>(password), password_len, salt, OPS_LIMIT,
                         MEM_LIMIT, HASH_ALG);
}

auto EncryptBuf(unsigned char *out_data, unsigned char *header, const unsigned char *buf, uintmax_t buf_len,
                const unsigned char *key) -> int {
    if (buf_len > crypto_secretstream_xchacha20poly1305_MESSAGEBYTES_MAX) {
        return -1;
    }

    crypto_secretstream_xchacha20poly1305_state state;
    crypto_secretstream_xchacha20poly1305_init_push(&state, header, key);
    uint64_t out_len = 0;
    crypto_secretstream_xchacha20poly1305_push(&state, out_data, &out_len, buf, buf_len, nullptr, 0,
                                               crypto_secretstream_xchacha20poly1305_TAG_FINAL);
    if (out_len != buf_len + crypto_secretstream_xchacha20poly1305_ABYTES) {
        return -1;
    }

    return 0;
}

auto HashPassword(unsigned char *hash, const unsigned char *password) -> int {
    int password_len = strlen(const_cast<char *>(reinterpret_cast<const char *>(password)));
    if (password_len < crypto_pwhash_PASSWD_MIN || password_len > crypto_pwhash_PASSWD_MAX) {
        return -1;
    }

    return crypto_pwhash_str(reinterpret_cast<char *>(hash), reinterpret_cast<const char *>(password), password_len,
                             OPS_LIMIT, MEM_LIMIT);
}

auto DecryptBuf(unsigned char *out_data, uint64_t *out_len, unsigned char *header, unsigned char *encrypted_buf,
                uintmax_t buf_len, const unsigned char *key) -> int {
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

auto VerifyPasswordHash(const unsigned char *hash, const unsigned char *password) -> int {
    int password_len = strlen(const_cast<char *>(reinterpret_cast<const char *>(password)));
    if (password_len < crypto_pwhash_PASSWD_MIN || password_len > crypto_pwhash_PASSWD_MAX) {
        return -1;
    }

    return crypto_pwhash_str_verify(reinterpret_cast<const char *>(hash), reinterpret_cast<const char *>(password),
                                    password_len);
}

void GenerateSalt(unsigned char *salt) { randombytes_buf(reinterpret_cast<char *>(salt), SALT_LEN); }

void Memzero(void *const ptr, const size_t len) { sodium_memzero(ptr, len); }

void SecureCpyStrToBuf(std::string &str, unsigned char *buf) {
    memcpy(buf, str.c_str(), str.size());
    buf[str.size()] = '\0';
}
