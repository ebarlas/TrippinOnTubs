#include "Interpolator.h"

trippin::Interpolator::Interpolator(Uint32 duration, int magnitude)
        : firstTick(-1), duration(duration), magnitude(magnitude) {

}

void trippin::Interpolator::setDuration(Uint32 dur) {
    duration = dur;
}

void trippin::Interpolator::setMagnitude(int mag) {
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
