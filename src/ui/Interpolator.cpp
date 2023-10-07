#include "Interpolator.h"

trippin::Interpolator::Interpolator(const RenderClock &renderClock, int duration, int magnitude, int offset) :
        renderClock(renderClock),
        firstTick(renderClock.getTicks()),
        duration(duration),
        magnitude(magnitude),
        offset(offset) {

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
    auto e = elapsed();
    if (e < duration) {
        auto progress = static_cast<float>(e) / static_cast<float>(duration);
        return offset + static_cast<int>(static_cast<float>(magnitude) * interpolate(progress));
    }
    return offset + magnitude;
}

bool trippin::Interpolator::complete() const {
    return elapsed() >= duration;
}

unsigned int trippin::Interpolator::elapsed() const {
    return (renderClock.getTicks() - firstTick).count();
}
