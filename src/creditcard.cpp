#include "creditcard.hpp"
#include "verification.hpp"

auto CreditCard::SetName(const std::string &name) -> int {
    if (!name.empty() && !ValidateInputAlnumOnly(name)) {
        return -1;
    }

    this->name_ = name;
    return 0;
}

auto CreditCard::SetCardNumber(const std::string &card_number) -> int {
    if (!ValidateInputDigitsOnly(card_number)) {
        return -1;
    }
    if (!ValidateCreditCardNumber(card_number)) {
        return -1;
    }

    this->card_number_ = card_number;
    this->DetermineNetwork();
    return 0;
}

auto CreditCard::SetCvv(const std::string &cvv) -> int {
    int cvv_len = cvv.size();
    if ((this->network_ == CreditCard::CARD_AMEX && cvv_len != 4) ||
        (this->network_ != CreditCard::CARD_AMEX && cvv_len != 3)) {
        return -1;
    }

    this->cvv_ = cvv;
    return 0;
}

auto CreditCard::SetMonth(const std::string &month) -> int {
    if (!ValidateInputInRange(month, 1, 12)) {
        return -1;
    }

    this->month_ = month;
    return 0;
}

auto CreditCard::SetYear(const std::string &year) -> int {
    if (!ValidateInputInRange(year, 1900, 2100)) {
        return -1;
    }

    this->year_ = year;
    return 0;
}

auto CreditCard::GetName() -> std::string {
    if (!this->name_.empty()) {
        return this->name_;
    }
    if (!this->default_name_.empty()) {
        return this->default_name_;
    }

    if (this->card_number_.empty()) {
        return "";
    }

    std::string last_four_digits = this->card_number_.substr(this->card_number_.size() - 4);
    this->default_name_ = this->GetNetworkString() + " " + last_four_digits;
    return this->default_name_;
}

auto CreditCard::FormatText() -> std::string {
    return this->name_ + "," + this->card_number_ + "," + this->cvv_ + "," + this->month_ + "," + this->year_ + ";";
}

void CreditCard::InitFromText(char *text) {
    char *rest = nullptr;
    char *field = strtok_r(text, ",", &rest);

    if (text[0] != ',') {
        this->SetName(std::string(field));
        field = strtok_r(nullptr, ",", &rest);
    }
    this->SetCardNumber(std::string(field));

    field = strtok_r(nullptr, ",", &rest);
    this->SetCvv(std::string(field));

    field = strtok_r(nullptr, ",", &rest);
    this->SetMonth(std::string(field));

    field = strtok_r(nullptr, ",", &rest);
    this->SetYear(std::string(field));
}

void CreditCard::DetermineNetwork() {
    switch (this->card_number_[0]) {
    case '4':
        this->network_ = CreditCard::CARD_VISA;
        break;
    case '2':
    case '5':
        this->network_ = CreditCard::CARD_MASTERCARD;
        break;
    case '3':
        this->network_ = CreditCard::CARD_AMEX;
        break;
    case '6':
        this->network_ = CreditCard::CARD_DISCOVER;
        break;
    default:
        this->network_ = CreditCard::CARD_OTHER;
    }
}

auto CreditCard::GetNetworkString() -> std::string {
    switch (this->network_) {
    case CreditCard::CARD_OTHER:
        return "Other";
    case CARD_VISA:
        return "Visa";
    case CARD_MASTERCARD:
        return "Mastercard";
    case CARD_AMEX:
        return "American Express";
    case CARD_DISCOVER:
        return "Discover";
    }
}
