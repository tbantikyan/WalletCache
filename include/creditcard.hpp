#ifndef CREDITCARD_HPP
#define CREDITCARD_HPP

#include <string>

class CreditCard {
    friend class CreditCardTest;

  public:
    auto SetName(const std::string &name) -> int;
    auto SetCardNumber(const std::string &card_number) -> int;
    auto SetCvv(const std::string &cvv) -> int;
    auto SetMonth(const std::string &month) -> int;
    auto SetYear(const std::string &year) -> int;

    auto GetName() -> std::string;

    auto FormatText() -> std::string;
    void InitFromText(char *text);

  private:
    enum CardNetwork {
        CARD_OTHER = 0,
        CARD_VISA,
        CARD_MASTERCARD,
        CARD_AMEX,
        CARD_DISCOVER,
    };

    std::string card_number_;
    std::string cvv_;
    std::string month_;
    std::string year_;
    CardNetwork network_;

    std::string name_;
    std::string default_name_;

    void DetermineNetwork();
    auto GetNetworkString() -> std::string;
};

#endif // CREDITCARD_HPP
