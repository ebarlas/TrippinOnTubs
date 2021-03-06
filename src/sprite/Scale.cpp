#include <cmath>
#include "Scale.h"

trippin::Scale::Scale(std::string name, double multiplier) : name(std::move(name)), multiplier(multiplier) {

}

const std::string &trippin::Scale::getName() const {
    return name;
}

double trippin::Scale::getMultiplier() const {
    return multiplier;
}
