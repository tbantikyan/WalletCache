#include "creditcard.hpp"
#include "verification.hpp"

int CreditCard::SetName(string name) {
    if (name.size() != 0 && !ValidateInputAlnumOnly(name)) {
        return -1;
    }

    this->name_ = name;
    return 0;
}

int CreditCard::SetCardNumber(string card_number) {
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

int CreditCard::SetCvv(string cvv) {
    int cvv_len = cvv.size();
    if ((this->network_ == CreditCard::CARD_AMEX && cvv_len != 4) ||
        (this->network_ != CreditCard::CARD_AMEX && cvv_len != 3)) {
        return -1;
    }

    this->cvv_ = cvv;
    return 0;
}

int CreditCard::SetMonth(string month) {
    if (!ValidateInputInRange(month, 1, 12)) {
        return -1;
    }

    this->month_ = month;
    return 0;
}

int CreditCard::SetYear(string year) {
    if (!ValidateInputInRange(year, 1900, 2100)) {
        return -1;
    }

    this->year_ = year;
    return 0;
}

string CreditCard::GetName() {
    if (this->name_ != "") {
        return this->name_;
    } else if (this->default_name_ != "") {
        return this->default_name_;
    }

    if (this->card_number_ == "") {
        return "";
    }

    string last_four_digits = this->card_number_.substr(this->card_number_.size() - 4);
    this->default_name_ = this->GetNetworkString() + " " + last_four_digits;
    return default_name_;
}

string CreditCard::FormatText() {
    return this->name_ + "," + this->card_number_ + "," + this->cvv_ + "," + this->month_ + "," + this->year_ + ";";
}

void CreditCard::InitFromText(char *text) {
    char *rest = nullptr;
    char *field = strtok_r(text, ",", &rest);

    if (text[0] != ',') {
        this->SetName(string(field));
        field = strtok_r(nullptr, ",", &rest);
    }
    this->SetCardNumber(string(field));

    field = strtok_r(nullptr, ",", &rest);
    this->SetCvv(string(field));

    field = strtok_r(nullptr, ",", &rest);
    this->SetMonth(string(field));

    field = strtok_r(nullptr, ",", &rest);
    this->SetYear(string(field));
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

string CreditCard::GetNetworkString() {
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
