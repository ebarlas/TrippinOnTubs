#include "Activation.h"

void trippin::Activation::setActivationProximity(int proximity) {
    activationProximity = proximity;
}

void trippin::Activation::setDeactivationProximity(int proximity) {
    deactivationProximity = proximity;
}

void trippin::Activation::setGoggin(const Goggin *g) {
    goggin = g;
}

void trippin::Activation::setUniverse(const trippin::Point<int> &uni) {
    universe = {0, 0, uni.x, uni.y};
}

bool trippin::Activation::shouldActivate(const Rect<int> &rect) const {
    return goggin->roundedPosition.x >= rect.x - activationProximity;
}

bool trippin::Activation::shouldDeactivate(const Rect<int> &rect) const {
    return goggin->roundedPosition.x >= rect.x + deactivationProximity || !universe.intersect(rect);
}
