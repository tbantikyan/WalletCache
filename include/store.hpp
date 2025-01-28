#ifndef STORE_HPP
#define STORE_HPP

#include "creditcard.hpp"
#include "crypto.hpp"
#include <fstream>
#include <string>
#include <vector>

using namespace std;

class Store {
  public:
    enum LoadStoreStatus {
        LOAD_STORE_VALID = 0,
        LOAD_STORE_OPEN_ERR,
        LOAD_STORE_HEADER_READ_ERR,
        LOAD_STORE_PWD_VERIFY_ERR,
        LOAD_STORE_KEY_DERIVATION_ERR,
        LOAD_STORE_DATA_READ_ERR,
        LOAD_STORE_DATA_DECRYPT_ERR,
    };
    enum SaveStoreStatus {
        SAVE_STORE_VALID = 0,
        SAVE_STORE_OPEN_ERR,
        SAVE_STORE_HEADER_ERR,
        SAVE_STORE_WRITE_DATA_ERR,
        SAVE_STORE_COMMIT_TEMP_ERR,
    };

    ~Store();

    int init_new_store(unsigned char *password);
    LoadStoreStatus load_store(unsigned char *password);
    SaveStoreStatus save_store();
    void add_card(CreditCard *card);

    bool store_exists(bool is_tmp);
    int delete_store(bool is_tmp);

    string cards_display_string();

  private:
    vector<struct CreditCard *> cards;

    unsigned char hashed_password[HASH_LEN];
    unsigned char salt[SALT_LEN];
    unsigned char encryption_key[ENCYRPTION_KEY_LEN];

    const string STORE_FILE_NAME = "WalletCache.store";
    const string TMP_STORE_FILE_NAME = "WalletCache.store.tmp";

    ifstream in_stream;
    ofstream out_stream;

    int read_header(unsigned char *hash, unsigned char *salt);
    int read_data(unsigned char *decrypted_data, uintmax_t data_size, unsigned long long *decrypted_size_actual);
    int write_header(const unsigned char *hash, const unsigned char *salt);
    int write_data(unsigned char *data, uintmax_t data_size);

    uintmax_t get_cards_size();
    uintmax_t cards_formatted(unsigned char *buf);
    void load_cards(unsigned char* data);

    uintmax_t get_store_size(bool is_tmp);
    int open_store_in(bool is_tmp);
    int open_store_out(bool is_tmp);
    int get_store_path(string &path, bool is_tmp);
    int commit_temp();
};

#endif // STORE_HPP
