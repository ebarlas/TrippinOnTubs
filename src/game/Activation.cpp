#include "Activation.h"

trippin::Activation::Activation(
        int activationProximity,
        int deactivationProximity,
        const trippin::Goggin &goggin,
        trippin::Rect<int> universe) :
        activationProximity(activationProximity),
        deactivationProximity(deactivationProximity),
        goggin(goggin),
        universe(universe) {}

bool trippin::Activation::shouldActivate(const Rect<int> &rect) const {
    return goggin.roundedPosition.x >= rect.x - activationProximity;
}

bool trippin::Activation::shouldDeactivate(const Rect<int> &rect) const {
    return goggin.roundedPosition.x >= rect.x + deactivationProximity || !universe.intersect(rect);
}
