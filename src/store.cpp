#include "store.hpp"
#include "crypto.hpp"
#include "utils.hpp"

#include <cstring>
#include <filesystem>

Store::~Store() {
    for (CreditCard *card_ptr : this->cards_) {
        free(card_ptr);
    }

    this->cards_.clear();
}

auto Store::InitNewStore(unsigned char *password) -> int {
    unsigned char hash[HASH_LEN];
    if (HashPassword(hash, password) != 0) {
        return -1;
    }

    unsigned char salt[SALT_LEN];
    GenerateSalt(salt);

    if (this->OpenStoreOut(false) != 0) {
        return -1;
    }

    if (this->WriteHeader(hash, salt) != 0) {
        this->out_stream_.close();
        return -1;
    }
    this->out_stream_.close();

    Memzero(hash, HASH_LEN);
    return 0;
}

auto Store::LoadStore(unsigned char *password) -> Store::LoadStoreStatus {
    unsigned char hash[HASH_LEN];
    unsigned char salt[SALT_LEN];
    if (this->OpenStoreIn(false) != 0) {
        return LOAD_STORE_OPEN_ERR;
    }

    if (this->ReadHeader(hash, salt) != 0) {
        return LOAD_STORE_HEADER_READ_ERR;
    }

    if (VerifyPasswordHash(hash, password) != 0) {
        return LOAD_STORE_PWD_VERIFY_ERR;
    }

    unsigned char encryption_key[ENCYRPTION_KEY_LEN];
    if (DeriveEncryptionKey(encryption_key, ENCYRPTION_KEY_LEN, password, salt) != 0) {
        return LOAD_STORE_KEY_DERIVATION_ERR;
    }

    memcpy(this->hashed_password_, hash, HASH_LEN);
    memcpy(this->salt_, salt, SALT_LEN);
    memcpy(this->encryption_key_, encryption_key, ENCYRPTION_KEY_LEN);

    uintmax_t store_size = this->GetStoreSize(false);
    if (store_size == 0) {
        return LOAD_STORE_DATA_READ_ERR;
    }
    uintmax_t data_size = store_size - HASH_LEN - SALT_LEN;
    if (data_size == 0) {
        return LOAD_STORE_VALID;
    }

    auto *decrypted_data = static_cast<unsigned char *>(malloc(data_size));
    uint64_t decrypted_size_actual;
    if (this->ReadData(decrypted_data, data_size, &decrypted_size_actual) != 0) {
        return LOAD_STORE_DATA_DECRYPT_ERR;
    }
    decrypted_data[decrypted_size_actual] = 0;

    this->LoadCards(decrypted_data);

    Memzero(encryption_key, ENCYRPTION_KEY_LEN);
    Memzero(decrypted_data, data_size);
    free(decrypted_data);
    this->in_stream_.close();

    return LOAD_STORE_VALID;
}

auto Store::SaveStore() -> Store::SaveStoreStatus {
    if (this->OpenStoreOut(true) != 0) {
        return SAVE_STORE_OPEN_ERR;
    }
    if (this->WriteHeader(this->hashed_password_, this->salt_) != 0) {
        this->out_stream_.close();
        return SAVE_STORE_HEADER_ERR;
    }

    uintmax_t data_size = this->GetCardsSize();
    if (data_size != 0) {
        unsigned char data[data_size];
        this->CardsFormatted(data);
        if (this->WriteData(data, data_size) != 0) {
            return SAVE_STORE_WRITE_DATA_ERR;
        }
        Memzero(data, data_size);
    }

    this->out_stream_.close();

    if (this->CommitTemp() != 0) {
        return SAVE_STORE_COMMIT_TEMP_ERR;
    }

    return SAVE_STORE_VALID;
}

void Store::AddCard(CreditCard *card) { this->cards_.push_back(card); }

auto Store::StoreExists(bool is_tmp) -> bool {
    std::string store_path;
    if (this->GetStorePath(store_path, is_tmp) != 0) {
        return false;
    }

    return CheckFileExists(store_path);
}

auto Store::DeleteStore(bool is_tmp) -> int {
    if (!this->StoreExists(is_tmp)) {
        return -1;
    }

    std::string store_path;
    if (this->GetStorePath(store_path, is_tmp) != 0) {
        return -1;
    }

    if (!std::filesystem::remove(store_path)) {
        return -1;
    }
    return 0;
}

auto Store::CardsDisplayString() -> std::string {
    std::string result;
    for (CreditCard *card : this->cards_) {
        result += card->GetName() + "\n";
    }
    return result;
}

auto Store::ReadHeader(unsigned char *hash, unsigned char *salt) -> int {
    if (this->in_stream_.tellg() != 0) {
        return -1;
    }

    this->in_stream_.read(reinterpret_cast<char *>(hash), HASH_LEN);
    if (!this->in_stream_) {
        return -1;
    }
    this->in_stream_.read(reinterpret_cast<char *>(salt), SALT_LEN);
    if (!this->in_stream_) {
        return -1;
    }

    if (this->in_stream_.tellg() != (HASH_LEN + SALT_LEN)) {
        return -1;
    }

    return 0;
}

auto Store::ReadData(unsigned char *decrypted_data, uintmax_t data_size, uint64_t *decrypted_size_actual) -> int {
    unsigned char header[ENCRYPTION_HEADER_LEN];
    uintmax_t encrypted_data_size = data_size - ENCRYPTION_HEADER_LEN;
    auto *encrypted_data = static_cast<unsigned char *>(malloc(encrypted_data_size));

    this->in_stream_.read(reinterpret_cast<char *>(header), ENCRYPTION_HEADER_LEN);
    if (!this->in_stream_) {
        free(encrypted_data);
        return -1;
    }
    this->in_stream_.read(reinterpret_cast<char *>(encrypted_data), encrypted_data_size);
    if (!this->in_stream_) {
        free(encrypted_data);
        return -1;
    }

    if (DecryptBuf(decrypted_data, decrypted_size_actual, header, encrypted_data, encrypted_data_size,
                   this->encryption_key_) != 0) {
        free(encrypted_data);
        return -1;
    }

    free(encrypted_data);
    return 0;
}

auto Store::WriteHeader(const unsigned char *hash, const unsigned char *salt) -> int {
    if (this->out_stream_.tellp() != 0) {
        this->out_stream_.close();
        return -1;
    }

    this->out_stream_.write(reinterpret_cast<const char *>(hash), HASH_LEN);
    this->out_stream_.write(reinterpret_cast<const char *>(salt), SALT_LEN);
    if (this->out_stream_.tellp() != (HASH_LEN + SALT_LEN)) {
        return -1;
    }

    return 0;
}

auto Store::WriteData(unsigned char *data, uintmax_t data_size) -> int {
    unsigned char header[ENCRYPTION_HEADER_LEN];
    uint64_t encrypted_len = data_size + ENCRYPTION_ADDED_BYTES;
    auto *encrypted_data = static_cast<unsigned char *>(malloc(encrypted_len));
    if (encrypted_data == nullptr) {
        return -1;
    }

    if (EncryptBuf(encrypted_data, header, data, data_size, this->encryption_key_) != 0) {
        free(encrypted_data);
        return -1;
    }

    this->out_stream_.write(reinterpret_cast<const char *>(header), sizeof(header));
    this->out_stream_.write(reinterpret_cast<const char *>(encrypted_data), encrypted_len);

    if (this->out_stream_.tellp() != (HASH_LEN + SALT_LEN + sizeof(header) + encrypted_len)) {
        free(encrypted_data);
        return -1;
    }

    free(encrypted_data);
    return 0;
}

auto Store::GetCardsSize() -> uintmax_t {
    uintmax_t total_size;
    for (CreditCard *card_ptr : this->cards_) {
        total_size += card_ptr->FormatText().size();
    }
    return total_size;
}

auto Store::CardsFormatted(unsigned char *buf) -> uintmax_t {
    uintmax_t pos = 0;
    for (CreditCard *card_ptr : this->cards_) {
        std::string formatted_card = card_ptr->FormatText();
        size_t formatted_len = formatted_card.size();
        memcpy(buf + pos, formatted_card.c_str(), formatted_len);
        pos += formatted_len;
    }

    return pos;
}

void Store::LoadCards(unsigned char *data) {
    char *rest = nullptr;
    char *portion = strtok_r(reinterpret_cast<char *>(data), ";", &rest);

    while (portion != nullptr) {
        auto *card = static_cast<CreditCard *>(calloc(1, sizeof(CreditCard)));
        card->InitFromText(portion);
        this->AddCard(card);

        portion = strtok_r(nullptr, ";", &rest);
    }
}

auto Store::GetStoreSize(bool is_tmp) -> uintmax_t {
    if (!this->StoreExists(is_tmp)) {
        return 0;
    }

    std::string store_path;
    if (this->GetStorePath(store_path, is_tmp) != 0) {
        return 0;
    }

    try {
        return std::filesystem::file_size(store_path);
    } catch (...) {
        return 0;
    }
}

auto Store::OpenStoreIn(bool is_tmp) -> int {
    std::string store_path;
    if (this->GetStorePath(store_path, is_tmp) != 0) {
        return -1;
    }

    try {
        this->in_stream_.open(store_path, std::ios::binary);
    } catch (...) {
        return -1;
    }

    return 0;
}

auto Store::OpenStoreOut(bool is_tmp) -> int {
    std::string store_path;
    if (this->GetStorePath(store_path, is_tmp) != 0) {
        return -1;
    }

    try {
        this->out_stream_.open(store_path, std::ios::binary);
    } catch (...) {
        return -1;
    }

    return 0;
}

auto Store::GetStorePath(std::string &path, bool is_tmp) -> int {
    std::string homepath = GetHomePath();
    if (homepath.empty()) {
        return -1;
    }

    std::string file = is_tmp ? this->tmp_store_file_name_ : this->store_file_name_;
    path = GetFilePath(homepath, file);
    return 0;
}

auto Store::CommitTemp() -> int {
    std::string perm_path;
    std::string temp_path;
    this->GetStorePath(perm_path, false);
    this->GetStorePath(temp_path, true);

    if (!this->StoreExists(false)) {
        if (rename(temp_path.c_str(), perm_path.c_str()) != 0) {
            this->DeleteStore(true);
            return -1;
        }
        return 0;
    }

    std::string backup_path = perm_path + ".bak";
    if (rename(perm_path.c_str(), backup_path.c_str()) != 0) {
        this->DeleteStore(true);
        return -1;
    }

    if (rename(temp_path.c_str(), perm_path.c_str()) != 0) {
        rename(backup_path.c_str(), perm_path.c_str());
        this->DeleteStore(true);
        return -1;
    }

    if (!std::filesystem::remove(backup_path)) {
        return -1;
    }

    return 0;
}
