#include "ExitOverlay.h"

trippin::ExitOverlay::ExitOverlay(
        const Point<int> &windowSize,
        int margin,
        const Sprite &sprite,
        const RenderClock &renderClock,
        int slideDuration,
        int pauseDuration) :
        sprite(sprite),
        slideDuration(slideDuration),
        pauseDuration(pauseDuration),
        top{windowSize.y - sprite.getDeviceSize().y - margin},
        direction(Direction::in),
        inInterpolator(renderClock, slideDuration, margin + sprite.getDeviceSize().x, -sprite.getDeviceSize().x),
        outInterpolator(renderClock, slideDuration, -(margin + sprite.getDeviceSize().x), margin) {}

void trippin::ExitOverlay::reset() {
    direction = Direction::in;
    inInterpolator.reset();
}

void trippin::ExitOverlay::render() {
    if (direction == Direction::in && inInterpolator.complete()) {
        direction = Direction::pause;
    }
    if (pauseDuration && direction == Direction::pause && inInterpolator.elapsed() >= slideDuration + pauseDuration) {
        direction = Direction::out;
        outInterpolator.reset();
    }
    sprite.renderDevice({interpolate(), top}, 0);
}

bool trippin::ExitOverlay::exitClicked(const trippin::Point<int> &coords) const {
    auto dims = sprite.getDeviceSize();
    Rect<int> r{interpolate(), top, dims.x, dims.y};
    return r.contains(coords);
}

int trippin::ExitOverlay::interpolate() const {
    return direction == Direction::in || direction == Direction::pause
           ? inInterpolator.interpolate()
           : outInterpolator.interpolate();;
}