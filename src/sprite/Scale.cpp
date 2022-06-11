#include <cmath>
#include "Scale.h"

trippin::Scale::Scale(std::string name, int multiplier) : name(std::move(name)), multiplier(multiplier) {

}

const std::string &trippin::Scale::getName() const {
    return name;
}

int trippin::Scale::getMultiplier() const {
    return multiplier;
}
