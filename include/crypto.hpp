#ifndef CRYPTO_HPP
#define CRYPTO_HPP

#include <sodium.h>
#include <stdlib.h>

#define HASH_LEN (crypto_pwhash_STRBYTES)
#define SALT_LEN (crypto_pwhash_SALTBYTES)

int init_crypto();
int hash_password(unsigned char *hash, const unsigned char *password);
void generate_salt(unsigned char *salt);
void memzero(void *const ptr, const size_t len);
#endif // CRYPTO_HPP
