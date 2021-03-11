#include "Interpolator.h"

void trippin::Interpolator::init(Uint32 dur, int mag) {
    firstTick = -1;
    duration = dur;
    magnitude = mag;
}

void trippin::Interpolator::reset() {
    firstTick = SDL_GetTicks();
}

float trippin::Interpolator::interpolate(float input) {
    return (float) (1.0f - (1.0f - input) * (1.0f - input));
}

int trippin::Interpolator::interpolate() const {
    auto elapsed = SDL_GetTicks() - firstTick;
    if (elapsed < duration) {
        float progress = static_cast<float>(elapsed) / duration;
        return static_cast<int>(static_cast<float>(magnitude) * interpolate(progress));
    }
    return magnitude;
}
