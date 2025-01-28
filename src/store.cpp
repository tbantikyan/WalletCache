#include "store.hpp"
#include "crypto.hpp"
#include "utils.hpp"

#include <cstring>
#include <filesystem>

Store::~Store() {
    for (CreditCard *card_ptr : this->cards) {
        free(card_ptr);
    }

    this->cards.clear();
}

int Store::init_new_store(unsigned char *password) {
    unsigned char hash[HASH_LEN];
    if (hash_password(hash, password) != 0) {
        return -1;
    }

    unsigned char salt[SALT_LEN];
    generate_salt(salt);

    if (this->open_store_out(false) != 0) {
        return -1;
    }

    if (this->write_header(hash, salt) != 0) {
        this->out_stream.close();
        return -1;
    }
    this->out_stream.close();

    memzero(hash, HASH_LEN);
    return 0;
}

Store::LoadStoreStatus Store::load_store(unsigned char *password) {
    unsigned char hash[HASH_LEN];
    unsigned char salt[SALT_LEN];
    if (this->open_store_in(false) != 0) {
        return LOAD_STORE_OPEN_ERR;
    }

    if (this->read_header(hash, salt) != 0) {
        return LOAD_STORE_HEADER_READ_ERR;
    }

    if (verify_password_hash(hash, password) != 0) {
        return LOAD_STORE_PWD_VERIFY_ERR;
    }

    unsigned char encryption_key[ENCYRPTION_KEY_LEN];
    if (derive_encryption_key(encryption_key, ENCYRPTION_KEY_LEN, password, salt) != 0) {
        return LOAD_STORE_KEY_DERIVATION_ERR;
    }

    memcpy(this->hashed_password, hash, HASH_LEN);
    memcpy(this->salt, salt, SALT_LEN);
    memcpy(this->encryption_key, encryption_key, ENCYRPTION_KEY_LEN);

    uintmax_t store_size = this->get_store_size(false);
    if (store_size == 0) {
        return LOAD_STORE_DATA_READ_ERR;
    }
    uintmax_t data_size = store_size - HASH_LEN - SALT_LEN;
    if (data_size == 0) {
        return LOAD_STORE_VALID;
    }

    unsigned char *decrypted_data = (unsigned char *)malloc(data_size);
    unsigned long long decrypted_size_actual;
    if (this->read_data(decrypted_data, data_size, &decrypted_size_actual) != 0) {
        return LOAD_STORE_DATA_DECRYPT_ERR;
    }
    decrypted_data[decrypted_size_actual] = 0;

    this->load_cards(decrypted_data);

    memzero(encryption_key, ENCYRPTION_KEY_LEN);
    memzero(decrypted_data, data_size);
    free(decrypted_data);
    this->in_stream.close();

    return LOAD_STORE_VALID;
}

Store::SaveStoreStatus Store::save_store() {
    if (this->open_store_out(true) != 0) {
        return SAVE_STORE_OPEN_ERR;
    }
    if (this->write_header(this->hashed_password, this->salt) != 0) {
        this->out_stream.close();
        return SAVE_STORE_HEADER_ERR;
    }

    uintmax_t data_size = this->get_cards_size();
    if (data_size != 0) {
        unsigned char data[data_size];
        this->cards_formatted(data);
        if (this->write_data(data, data_size) != 0) {
            return SAVE_STORE_WRITE_DATA_ERR;
        }
        memzero(data, data_size);
    }

    this->out_stream.close();

    if (this->commit_temp() != 0) {
        return SAVE_STORE_COMMIT_TEMP_ERR;
    }

    return SAVE_STORE_VALID;
}

void Store::add_card(CreditCard *card) { this->cards.push_back(card); }

bool Store::store_exists(bool is_tmp) {
    string store_path;
    if (this->get_store_path(store_path, is_tmp) != 0) {
        return -1;
    }

    return check_file_exists(store_path);
}

int Store::delete_store(bool is_tmp) {
    if (!this->store_exists(is_tmp)) {
        return -1;
    }

    string store_path;
    if (this->get_store_path(store_path, is_tmp) != 0) {
        return -1;
    }

    if (!std::filesystem::remove(store_path)) {
        return -1;
    }
    return 0;
}

string Store::cards_display_string() {
    string result = "";
    for (CreditCard *card : this->cards) {
        result += card->get_name() + "\n";
    }
    return result;
}

int Store::read_header(unsigned char *hash, unsigned char *salt) {
    if (this->in_stream.tellg() != 0) {
        return -1;
    }

    this->in_stream.read((char *)hash, HASH_LEN);
    if (!this->in_stream) {
        return -1;
    }
    this->in_stream.read((char *)salt, SALT_LEN);
    if (!this->in_stream) {
        return -1;
    }

    if (this->in_stream.tellg() != (HASH_LEN + SALT_LEN)) {
        return -1;
    }

    return 0;
}

int Store::read_data(unsigned char *decrypted_data, uintmax_t data_size, unsigned long long *decrypted_size_actual) {
    unsigned char header[ENCRYPTION_HEADER_LEN];
    uintmax_t encrypted_data_size = data_size - ENCRYPTION_HEADER_LEN;
    unsigned char *encrypted_data = (unsigned char *)malloc(encrypted_data_size);

    this->in_stream.read((char *)header, ENCRYPTION_HEADER_LEN);
    if (!this->in_stream) {
        free(encrypted_data);
        return -1;
    }
    this->in_stream.read((char *)encrypted_data, encrypted_data_size);
    if (!this->in_stream) {
        free(encrypted_data);
        return -1;
    }

    if (decrypt_buf(decrypted_data, decrypted_size_actual, header, encrypted_data, encrypted_data_size,
                    this->encryption_key) != 0) {
        free(encrypted_data);
        return -1;
    }

    free(encrypted_data);
    return 0;
}

int Store::write_header(const unsigned char *hash, const unsigned char *salt) {
    if (this->out_stream.tellp() != 0) {
        this->out_stream.close();
        return -1;
    }

    this->out_stream.write((char *)hash, HASH_LEN);
    this->out_stream.write((char *)salt, SALT_LEN);
    if (this->out_stream.tellp() != (HASH_LEN + SALT_LEN)) {
        return -1;
    }

    return 0;
}

int Store::write_data(unsigned char *data, uintmax_t data_size) {
    unsigned char header[ENCRYPTION_HEADER_LEN];
    unsigned long long encrypted_len = data_size + ENCRYPTION_ADDED_BYTES;
    unsigned char *encrypted_data = (unsigned char *)malloc(encrypted_len);
    if (encrypted_data == nullptr) {
        return -1;
    }

    if (encrypt_buf(encrypted_data, header, data, data_size, this->encryption_key) != 0) {
        free(encrypted_data);
        return -1;
    }

    this->out_stream.write((char *)header, sizeof(header));
    this->out_stream.write((char *)encrypted_data, encrypted_len);

    if ((size_t)this->out_stream.tellp() != (HASH_LEN + SALT_LEN + sizeof(header) + encrypted_len)) {
        free(encrypted_data);
        return -1;
    }

    free(encrypted_data);
    return 0;
}

uintmax_t Store::get_cards_size() {
    uintmax_t total_size;
    for (CreditCard *card_ptr : this->cards) {
        total_size += card_ptr->format_text().size();
    }
    return total_size;
}

uintmax_t Store::cards_formatted(unsigned char *buf) {
    uintmax_t pos = 0;
    for (CreditCard *card_ptr : this->cards) {
        string formatted_card = card_ptr->format_text();
        size_t formatted_len = formatted_card.size();
        memcpy(buf + pos, formatted_card.c_str(), formatted_len);
        pos += formatted_len;
    }

    return pos;
}

void Store::load_cards(unsigned char *data) {
    char *rest = nullptr;
    char *portion = strtok_r((char *)data, ";", &rest);

    while (portion) {
        CreditCard *card = (CreditCard *)calloc(1, sizeof(CreditCard));
        card->init_from_text(portion);
        this->add_card(card);

        portion = strtok_r(nullptr, ";", &rest);
    }
}

uintmax_t Store::get_store_size(bool is_tmp) {
    if (!this->store_exists(is_tmp)) {
        return 0;
    }

    string store_path;
    if (this->get_store_path(store_path, is_tmp) != 0) {
        return 0;
    }

    try {
        return std::filesystem::file_size(store_path);
    } catch (...) {
        return 0;
    }
}

int Store::open_store_in(bool is_tmp) {
    string store_path;
    if (this->get_store_path(store_path, is_tmp) != 0) {
        return -1;
    }

    try {
        this->in_stream.open(store_path, ios::binary);
    } catch (...) {
        return -1;
    }

    return 0;
}

int Store::open_store_out(bool is_tmp) {
    string store_path;
    if (this->get_store_path(store_path, is_tmp) != 0) {
        return -1;
    }

    try {
        this->out_stream.open(store_path, ios::binary);
    } catch (...) {
        return -1;
    }

    return 0;
}

int Store::get_store_path(string &path, bool is_tmp) {
    string homepath = get_home_path();
    if (homepath.empty()) {
        return -1;
    }

    string file = is_tmp ? this->TMP_STORE_FILE_NAME : this->STORE_FILE_NAME;
    path = get_file_path(homepath, file);
    return 0;
}

int Store::commit_temp() {
    string perm_path;
    string temp_path;
    this->get_store_path(perm_path, false);
    this->get_store_path(temp_path, true);

    if (!this->store_exists(false)) {
        if (rename(temp_path.c_str(), perm_path.c_str()) != 0) {
            this->delete_store(true);
            return -1;
        }
        return 0;
    }

    string backup_path = perm_path + ".bak";
    if (rename(perm_path.c_str(), backup_path.c_str()) != 0) {
        this->delete_store(true);
        return -1;
    }

    if (rename(temp_path.c_str(), perm_path.c_str()) != 0) {
        rename(backup_path.c_str(), perm_path.c_str());
        this->delete_store(true);
        return -1;
    }

    if (!std::filesystem::remove(backup_path) != 0) {
        return -1;
    }

    return 0;
}
