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

trippin::Point<int> trippin::Scale::scale(const trippin::Point<int> &p) const {
    return {static_cast<int>(std::round(p.x * multiplier)), static_cast<int>(std::round(p.y * multiplier))};
}
