#ifndef STORE_HPP
#define STORE_HPP

#include "crypto.hpp"
#include <fstream>
#include <string>

using namespace std;

class Store {
  public:
    enum LoadStoreStatus {
        LOAD_STORE_VALID = 0,
        DATA_READ_ERROR,
        PWD_VERIFY_ERR,
        KEY_DERIVATION_ERROR,
    };

    int init_new_store(unsigned char *password);
    LoadStoreStatus load_store(unsigned char *password);

    bool data_exists();
    int delete_data();

  private:
    unsigned char hashed_password[HASH_LEN];
    unsigned char salt[SALT_LEN];
    unsigned char encryption_key[ENCYRPTION_KEY_LEN];

    const string DATA_FILE_NAME = "WalletCache.data";
    ifstream in_stream;
    ofstream out_stream;
   
    int read_header(const unsigned char *hash, const unsigned char *salt);
    int write_header(const unsigned char *hash, const unsigned char *salt);

    int open_data_in();
    int open_data_out();
    int get_data_path(string &path);
};

#endif // STORE_HPP
