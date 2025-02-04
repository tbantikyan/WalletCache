#ifndef CREDITCARD_HPP
#define CREDITCARD_HPP

#include <string>

using namespace std;

class CreditCard {
    friend class CreditCardTest;

  public:
    int SetName(string name);
    int SetCardNumber(string card_number);
    int SetCvv(string cvv);
    int SetMonth(string month);
    int SetYear(string year);

    string GetName();

    string FormatText();
    void InitFromText(char *text);

  private:
    enum CardNetwork {
        CARD_OTHER = 0,
        CARD_VISA,
        CARD_MASTERCARD,
        CARD_AMEX,
        CARD_DISCOVER,
    };

    string card_number_;
    string cvv_;
    string month_;
    string year_;
    CardNetwork network_;

    string name_;
    string default_name_;

    void DetermineNetwork();
    string GetNetworkString();
};

#endif // CREDITCARD_HPP
