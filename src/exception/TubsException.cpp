#include "TubsException.h"

trippin::TubsException::TubsException(std::string &&message) : message(std::move(message)) {}

const std::string &trippin::TubsException::getMessage() const {
    return message;
}
