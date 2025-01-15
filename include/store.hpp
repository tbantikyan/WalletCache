#ifndef STORE_HPP
#define STORE_HPP

#include <fstream>
#include <string>

using namespace std;

class Store {
  public:
    int init_new_store(unsigned char *password);
    bool data_exists();
    int delete_data();

  private:
    const string DATA_FILE_NAME = "WalletCache.data";
    ifstream in_stream;
    ofstream out_stream;

    int get_data_path(string &path);
    int write_header(const unsigned char *hash, const unsigned char *salt);
};

#endif // STORE_HPP
