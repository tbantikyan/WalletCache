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

int Store::InitNewStore(unsigned char *password) {
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

Store::LoadStoreStatus Store::LoadStore(unsigned char *password) {
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

    unsigned char *decrypted_data = (unsigned char *)malloc(data_size);
    unsigned long long decrypted_size_actual;
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

Store::SaveStoreStatus Store::SaveStore() {
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

bool Store::StoreExists(bool is_tmp) {
    string store_path;
    if (this->GetStorePath(store_path, is_tmp) != 0) {
        return -1;
    }

    return CheckFileExists(store_path);
}

int Store::DeleteStore(bool is_tmp) {
    if (!this->StoreExists(is_tmp)) {
        return -1;
    }

    string store_path;
    if (this->GetStorePath(store_path, is_tmp) != 0) {
        return -1;
    }

    if (!std::filesystem::remove(store_path)) {
        return -1;
    }
    return 0;
}

string Store::CardsDisplayString() {
    string result = "";
    for (CreditCard *card : this->cards_) {
        result += card->GetName() + "\n";
    }
    return result;
}

int Store::ReadHeader(unsigned char *hash, unsigned char *salt) {
    if (this->in_stream_.tellg() != 0) {
        return -1;
    }

    this->in_stream_.read((char *)hash, HASH_LEN);
    if (!this->in_stream_) {
        return -1;
    }
    this->in_stream_.read((char *)salt, SALT_LEN);
    if (!this->in_stream_) {
        return -1;
    }

    if (this->in_stream_.tellg() != (HASH_LEN + SALT_LEN)) {
        return -1;
    }

    return 0;
}

int Store::ReadData(unsigned char *decrypted_data, uintmax_t data_size, unsigned long long *decrypted_size_actual) {
    unsigned char header[ENCRYPTION_HEADER_LEN];
    uintmax_t encrypted_data_size = data_size - ENCRYPTION_HEADER_LEN;
    unsigned char *encrypted_data = (unsigned char *)malloc(encrypted_data_size);

    this->in_stream_.read((char *)header, ENCRYPTION_HEADER_LEN);
    if (!this->in_stream_) {
        free(encrypted_data);
        return -1;
    }
    this->in_stream_.read((char *)encrypted_data, encrypted_data_size);
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

int Store::WriteHeader(const unsigned char *hash, const unsigned char *salt) {
    if (this->out_stream_.tellp() != 0) {
        this->out_stream_.close();
        return -1;
    }

    this->out_stream_.write((char *)hash, HASH_LEN);
    this->out_stream_.write((char *)salt, SALT_LEN);
    if (this->out_stream_.tellp() != (HASH_LEN + SALT_LEN)) {
        return -1;
    }

    return 0;
}

int Store::WriteData(unsigned char *data, uintmax_t data_size) {
    unsigned char header[ENCRYPTION_HEADER_LEN];
    unsigned long long encrypted_len = data_size + ENCRYPTION_ADDED_BYTES;
    unsigned char *encrypted_data = (unsigned char *)malloc(encrypted_len);
    if (encrypted_data == nullptr) {
        return -1;
    }

    if (EncryptBuf(encrypted_data, header, data, data_size, this->encryption_key_) != 0) {
        free(encrypted_data);
        return -1;
    }

    this->out_stream_.write((char *)header, sizeof(header));
    this->out_stream_.write((char *)encrypted_data, encrypted_len);

    if ((size_t)this->out_stream_.tellp() != (HASH_LEN + SALT_LEN + sizeof(header) + encrypted_len)) {
        free(encrypted_data);
        return -1;
    }

    free(encrypted_data);
    return 0;
}

uintmax_t Store::GetCardsSize() {
    uintmax_t total_size;
    for (CreditCard *card_ptr : this->cards_) {
        total_size += card_ptr->FormatText().size();
    }
    return total_size;
}

uintmax_t Store::CardsFormatted(unsigned char *buf) {
    uintmax_t pos = 0;
    for (CreditCard *card_ptr : this->cards_) {
        string formatted_card = card_ptr->FormatText();
        size_t formatted_len = formatted_card.size();
        memcpy(buf + pos, formatted_card.c_str(), formatted_len);
        pos += formatted_len;
    }

    return pos;
}

void Store::LoadCards(unsigned char *data) {
    char *rest = nullptr;
    char *portion = strtok_r((char *)data, ";", &rest);

    while (portion) {
        CreditCard *card = (CreditCard *)calloc(1, sizeof(CreditCard));
        card->InitFromText(portion);
        this->AddCard(card);

        portion = strtok_r(nullptr, ";", &rest);
    }
}

uintmax_t Store::GetStoreSize(bool is_tmp) {
    if (!this->StoreExists(is_tmp)) {
        return 0;
    }

    string store_path;
    if (this->GetStorePath(store_path, is_tmp) != 0) {
        return 0;
    }

    try {
        return std::filesystem::file_size(store_path);
    } catch (...) {
        return 0;
    }
}

int Store::OpenStoreIn(bool is_tmp) {
    string store_path;
    if (this->GetStorePath(store_path, is_tmp) != 0) {
        return -1;
    }

    try {
        this->in_stream_.open(store_path, ios::binary);
    } catch (...) {
        return -1;
    }

    return 0;
}

int Store::OpenStoreOut(bool is_tmp) {
    string store_path;
    if (this->GetStorePath(store_path, is_tmp) != 0) {
        return -1;
    }

    try {
        this->out_stream_.open(store_path, ios::binary);
    } catch (...) {
        return -1;
    }

    return 0;
}

int Store::GetStorePath(string &path, bool is_tmp) {
    string homepath = GetHomePath();
    if (homepath.empty()) {
        return -1;
    }

    string file = is_tmp ? this->TMP_STORE_FILE_NAME_ : this->STORE_FILE_NAME_;
    path = GetFilePath(homepath, file);
    return 0;
}

int Store::CommitTemp() {
    string perm_path;
    string temp_path;
    this->GetStorePath(perm_path, false);
    this->GetStorePath(temp_path, true);

    if (!this->StoreExists(false)) {
        if (rename(temp_path.c_str(), perm_path.c_str()) != 0) {
            this->DeleteStore(true);
            return -1;
        }
        return 0;
    }

    string backup_path = perm_path + ".bak";
    if (rename(perm_path.c_str(), backup_path.c_str()) != 0) {
        this->DeleteStore(true);
        return -1;
    }

    if (rename(temp_path.c_str(), perm_path.c_str()) != 0) {
        rename(backup_path.c_str(), perm_path.c_str());
        this->DeleteStore(true);
        return -1;
    }

    if (!std::filesystem::remove(backup_path) != 0) {
        return -1;
    }

    return 0;
}
