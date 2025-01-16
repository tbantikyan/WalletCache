#include "store.hpp"
#include "crypto.hpp"
#include "utils.hpp"

#include <cstring>

int Store::init_new_store(unsigned char *password) {
    unsigned char hash[HASH_LEN];
    if (hash_password(hash, password) != 0) {
        return -1;
    }

    unsigned char salt[SALT_LEN];
    generate_salt(salt);

    if (this->write_header(hash, salt) != 0) {
        return -1;
    }
    memzero(hash, HASH_LEN);

    return 0;
}

Store::LoadStoreStatus Store::load_store(unsigned char *password) {
    unsigned char hash[HASH_LEN];
    unsigned char salt[SALT_LEN];
    if (this->read_header(hash, salt) != 0) {
        return DATA_READ_ERROR;
    }

    if (verify_password_hash(hash, password) != 0) {
        return PWD_VERIFY_ERR;
    }

    unsigned char encryption_key[ENCYRPTION_KEY_LEN];
    if (derive_encryption_key(encryption_key, ENCYRPTION_KEY_LEN, password, salt) != 0) {
        return KEY_DERIVATION_ERROR;
    }

    memcpy(this->hashed_password, hash, HASH_LEN);
    memcpy(this->salt, salt, SALT_LEN);
    memcpy(this->encryption_key, encryption_key, ENCYRPTION_KEY_LEN);
    memzero(encryption_key, ENCYRPTION_KEY_LEN);

    return LOAD_STORE_VALID;
}

bool Store::data_exists() {
    string data_path;
    if (this->get_data_path(data_path) != 0) {
        return -1;
    }

    return check_file_exists(data_path);
}

int Store::delete_data() {
    if (!this->data_exists()) {
        return -1;
    }

    string data_path;
    if (this->get_data_path(data_path) != 0) {
        return -1;
    }

    if (!std::filesystem::remove(data_path)) {
        return -1;
    }
    return 0;
}

int Store::read_header(const unsigned char *hash, const unsigned char *salt) {
    if (this->open_data_in() != 0) {
        return -1;
    }

    if (this->in_stream.tellg() != 0) {
        this->in_stream.close();
        return -1;
    }

    this->in_stream.read((char *)hash, HASH_LEN);
    if (!this->in_stream) {
        this->in_stream.close();
        return -1;
    }
    this->in_stream.read((char *)salt, SALT_LEN);
    if (!this->in_stream) {
        this->in_stream.close();
        return -1;
    }

    if (this->in_stream.tellg() != (HASH_LEN + SALT_LEN)) {
        this->in_stream.close();
        return -1;
    }

    this->in_stream.close();
    return 0;
}

int Store::write_header(const unsigned char *hash, const unsigned char *salt) {
    if (this->open_data_out() != 0) {
        return -1;
    }

    if (this->out_stream.tellp() != 0) {
        this->out_stream.close();
        return -1;
    }

    this->out_stream.write((char *)hash, HASH_LEN);
    this->out_stream.write((char *)salt, SALT_LEN);
    if (this->out_stream.tellp() != (HASH_LEN + SALT_LEN)) {
        this->out_stream.close();
        return -1;
    }

    this->out_stream.close();
    return 0;
}

int Store::open_data_in() {
    string data_path;
    if (this->get_data_path(data_path) != 0) {
        return -1;
    }

    try {
        this->in_stream.open(data_path, ios::binary);
    } catch (...) {
        return -1;
    }

    return 0;
}

int Store::open_data_out() {
    string data_path;
    if (this->get_data_path(data_path) != 0) {
        return -1;
    }

    try {
        this->out_stream.open(data_path, ios::binary);
    } catch (...) {
        return -1;
    }

    return 0;
}

int Store::get_data_path(string &path) {
    string homepath = get_home_path();
    if (homepath.empty()) {
        return -1;
    }

    path = get_file_path(homepath, this->DATA_FILE_NAME);
    return 0;
}
