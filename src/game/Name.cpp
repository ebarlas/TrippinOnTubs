#include "Name.h"

int &trippin::Name::operator[](int n) {
    return name[n];
}

std::string trippin::Name::encode() {
    std::string result;
    for (auto ch : name) {
        result.push_back(ch + 'A');
    }
    return result;
}

void trippin::Name::decode(const std::string &str) {
    for (int i = 0; i < str.size(); i++) {
        name[i] = str[i] - 'A';
    }
}
