#include "creditcard.hpp"
#include "verification.hpp"

int CreditCard::set_name(string name) {
    if (name.size() != 0 && !validate_input_alnum_only(name)) {
        return -1;
    }

    this->name = name;
    return 0;
}

int CreditCard::set_card_number(string card_number) {
    if (!validate_input_digits_only(card_number)) {
        return -1;
    }
    if (!validate_credit_card_number(card_number)) {
        return -1;
    }

    this->card_number = card_number;
    this->determine_network();
    return 0;
}

int CreditCard::set_cvv(string cvv) {
    int cvv_len = cvv.size();
    if ((this->network == CreditCard::CARD_AMEX && cvv_len != 4) ||
        (this->network != CreditCard::CARD_AMEX && cvv_len != 3)) {
        return -1;
    }

    this->cvv = cvv;
    return 0;
}

int CreditCard::set_month(string month) {
    if (!validate_input_in_range(month, 1, 12)) {
        return -1;
    }

    this->month = month;
    return 0;
}

int CreditCard::set_year(string year) {
    if (!validate_input_in_range(year, 1900, 2100)) {
        return -1;
    }

    this->year = year;
    return 0;
}

string CreditCard::get_name() {
    if (this->name != "") {
        return this->name;
    } else if (this->default_name != "") {
        return this->default_name;
    }

    if (this->card_number == "") {
        return "";
    }

    string last_four_digits = this->card_number.substr(this->card_number.size() - 4);
    this->default_name = this->get_network_string() + " " + last_four_digits;
    return default_name;
}

string CreditCard::format_text() {
    return this->name + "," + this->card_number + "," + this->cvv + "," + this->month + "," + this->year + ";";
}

void CreditCard::init_from_text(char *text) {
    char *rest = nullptr;
    char *field = strtok_r(text, ",", &rest);

    if (text[0] != ',') {
        this->set_name(string(field));
        field = strtok_r(nullptr, ",", &rest);
    }
    this->set_card_number(string(field));

    field = strtok_r(nullptr, ",", &rest);
    this->set_cvv(string(field));

    field = strtok_r(nullptr, ",", &rest);
    this->set_month(string(field));

    field = strtok_r(nullptr, ",", &rest);
    this->set_year(string(field));
}

void CreditCard::determine_network() {
    switch (this->card_number[0]) {
    case '4':
        this->network = CreditCard::CARD_VISA;
        break;
    case '2':
    case '5':
        this->network = CreditCard::CARD_MASTERCARD;
        break;
    case '3':
        this->network = CreditCard::CARD_AMEX;
        break;
    case '6':
        this->network = CreditCard::CARD_DISCOVER;
        break;
    default:
        this->network = CreditCard::CARD_OTHER;
    }
}

string CreditCard::get_network_string() {
    switch (this->network) {
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
