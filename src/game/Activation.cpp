#include "Activation.h"

void trippin::Activation::setProximity(int p) {
    proximity = p;
}

void trippin::Activation::setGoggin(const Goggin *g) {
    goggin = g;
}

bool trippin::Activation::shouldActivate(int position) const {
    return std::abs(position - goggin->roundedPosition.x) <= proximity;
}
