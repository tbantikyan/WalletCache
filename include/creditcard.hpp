#ifndef CREDITCARD_HPP
#define CREDITCARD_HPP

#include <string>

using namespace std;

class CreditCard {
    friend class CreditCardTest;

  public:
    int set_name(string name);
    int set_card_number(string card_number);
    int set_cvv(string cvv);
    int set_month(string month);
    int set_year(string year);

    string get_name();

    string format_text();
    void init_from_text(char *text);

  private:
    enum CardNetwork {
        CARD_OTHER = 0,
        CARD_VISA,
        CARD_MASTERCARD,
        CARD_AMEX,
        CARD_DISCOVER,
    };

    string card_number;
    string cvv;
    string month;
    string year;
    CardNetwork network;

    string name;
    string default_name;

    void determine_network();
    string get_network_string();
};

#endif // CREDITCARD_HPP
