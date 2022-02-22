#include "Interpolator.h"

trippin::Interpolator::Interpolator(const RenderClock &renderClock, int duration, int magnitude, int offset)
        : renderClock(renderClock), firstTick(-1), duration(duration), magnitude(magnitude), offset(offset) {

}

void trippin::Interpolator::setMagnitude(int mag) {
    magnitude = mag;
}

void trippin::Interpolator::setOffset(int off) {
    offset = off;
}

void trippin::Interpolator::reset() {
    firstTick = renderClock.getTicks();
}

float trippin::Interpolator::interpolate(float input) {
    return (float) (1.0f - (1.0f - input) * (1.0f - input));
}

int trippin::Interpolator::interpolate() const {
    auto elapsed = renderClock.getTicks() - firstTick;
    if (elapsed < duration) {
        float progress = static_cast<float>(elapsed) / duration;
        return static_cast<int>(static_cast<float>(magnitude) * interpolate(progress));
    }
    return offset + magnitude;
}
