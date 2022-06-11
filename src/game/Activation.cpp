#include "Activation.h"

void trippin::Activation::setActivationProximity(int_fast64_t proximity) {
    activationProximity = proximity;
}

void trippin::Activation::setDeactivationProximity(int_fast64_t proximity) {
    deactivationProximity = proximity;
}

void trippin::Activation::setGoggin(const Goggin *g) {
    goggin = g;
}

void trippin::Activation::setUniverse(const trippin::Point<int_fast64_t> &uni) {
    universe = {0, 0, uni.x, uni.y};
}

bool trippin::Activation::shouldActivate(const Rect<int_fast64_t> &rect) const {
    return goggin->position.x >= rect.x - activationProximity;
}

bool trippin::Activation::shouldDeactivate(const Rect<int_fast64_t> &rect) const {
    return goggin->position.x >= rect.x + deactivationProximity || !universe.intersect(rect);
}
