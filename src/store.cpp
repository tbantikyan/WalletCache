#include "store.hpp"
#include "crypto.hpp"
#include "utils.hpp"

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
    return 0;
}

bool Store::data_exists() {
    string data_path;
    if (this->get_data_path(data_path) != 0) {
        return -1;
    }
    
    return check_file_exists(data_path);
}

int Store::write_header(const unsigned char *hash, const unsigned char *salt) {
    string data_path;
    if (this->get_data_path(data_path) != 0) {
        return -1;
    }

    try {
        this->out_stream.open(data_path, ios::binary);
    } catch (...) {
        return -1;
    }
    if (this->out_stream.tellp() != 0) {
        return -1;
    }

    this->out_stream.write((char *)hash, HASH_LEN);
    this->out_stream.write((char *)salt, SALT_LEN);
    if (this->out_stream.tellp() != (HASH_LEN + SALT_LEN)) {
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
