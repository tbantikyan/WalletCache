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

    int InitNewStore(unsigned char *password);
    LoadStoreStatus LoadStore(unsigned char *password);
    SaveStoreStatus SaveStore();
    void AddCard(CreditCard *card);

    bool StoreExists(bool is_tmp);
    int DeleteStore(bool is_tmp);

    string CardsDisplayString();

  private:
    vector<struct CreditCard *> cards_;

    unsigned char hashed_password_[HASH_LEN];
    unsigned char salt_[SALT_LEN];
    unsigned char encryption_key_[ENCYRPTION_KEY_LEN];

    const string STORE_FILE_NAME_ = "WalletCache.store";
    const string TMP_STORE_FILE_NAME_ = "WalletCache.store.tmp";

    ifstream in_stream_;
    ofstream out_stream_;

    int ReadHeader(unsigned char *hash, unsigned char *salt);
    int ReadData(unsigned char *decrypted_data, uintmax_t data_size, unsigned long long *decrypted_size_actual);
    int WriteHeader(const unsigned char *hash, const unsigned char *salt);
    int WriteData(unsigned char *data, uintmax_t data_size);

    uintmax_t GetCardsSize();
    uintmax_t CardsFormatted(unsigned char *buf);
    void LoadCards(unsigned char *data);

    uintmax_t GetStoreSize(bool is_tmp);
    int OpenStoreIn(bool is_tmp);
    int OpenStoreOut(bool is_tmp);
    int GetStorePath(string &path, bool is_tmp);
    int CommitTemp();
};

#endif // STORE_HPP
