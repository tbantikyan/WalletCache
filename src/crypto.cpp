#include "crypto.hpp"

#include <string.h>

int hash_password(const unsigned char *hash, const unsigned char *password) {
    int password_len = strlen((char *)password);
    if (password_len < crypto_pwhash_PASSWD_MIN ||
        password_len > crypto_pwhash_PASSWD_MAX) {
        return -1;
    }

    return crypto_pwhash_str((char *)hash, (const char *)password, password_len,
                             crypto_pwhash_OPSLIMIT_SENSITIVE,
                             crypto_pwhash_MEMLIMIT_SENSITIVE);
}

void generate_salt(const unsigned char *salt) {
    randombytes_buf((char *)salt, SALT_LEN);
}
