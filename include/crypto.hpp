#ifndef CRYPTO_HPP
#define CRYPTO_HPP

#include <sodium.h>
#include <stdlib.h>

#define HASH_LEN (crypto_pwhash_STRBYTES)
#define SALT_LEN (crypto_pwhash_SALTBYTES)

int hash_password(const unsigned char *hash, const unsigned char *password);
void generate_salt(const unsigned char *salt);
#endif // CRYPTO_HPP
