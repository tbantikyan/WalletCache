#ifndef STORE_HPP
#define STORE_HPP

#include "creditcard.hpp"
#include "icrypto.hpp"
#include <fstream>
#include <string>
#include <vector>

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

    explicit Store(std::shared_ptr<ICrypto> crypto);
    ~Store();

    auto InitNewStore(unsigned char *password) -> int;
    auto LoadStore(unsigned char *password) -> LoadStoreStatus;
    auto SaveStore() -> SaveStoreStatus;
    void AddCard(CreditCard *card);

    auto StoreExists(bool is_tmp) -> bool;
    auto DeleteStore(bool is_tmp) -> int;

    auto CardsDisplayString() -> std::string;

  private:
    std::shared_ptr<ICrypto> crypto_;
    std::vector<struct CreditCard *> cards_;

    std::unique_ptr<unsigned char[]> hashed_password_;
    std::unique_ptr<unsigned char[]> salt_;
    std::unique_ptr<unsigned char[]> encryption_key_;

    const std::string store_file_name_ = "WalletCache.store";
    const std::string tmp_store_file_name_ = "WalletCache.store.tmp";

    std::ifstream in_stream_;
    std::ofstream out_stream_;

    auto ReadHeader(unsigned char *hash, unsigned char *salt) -> int;
    auto ReadData(unsigned char *decrypted_data, uintmax_t data_size, uint64_t *decrypted_size_actual) -> int;
    auto WriteHeader(const unsigned char *hash, const unsigned char *salt) -> int;
    auto WriteData(unsigned char *data, uintmax_t data_size) -> int;

    auto GetCardsSize() -> uintmax_t;
    auto CardsFormatted(unsigned char *buf) -> uintmax_t;
    void LoadCards(unsigned char *data);

    auto GetStoreSize(bool is_tmp) -> uintmax_t;
    auto OpenStoreIn(bool is_tmp) -> int;
    auto OpenStoreOut(bool is_tmp) -> int;
    auto GetStorePath(std::string &path, bool is_tmp) -> int;
    auto CommitTemp() -> int;
};

#endif // STORE_HPP
