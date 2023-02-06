#include "SimpleOverlay.h"

trippin::SimpleOverlay::SimpleOverlay(
        const Point<int> &windowSize,
        const Sprite &sprite,
        const RenderClock &renderClock) :
        windowSize(windowSize),
        sprite(sprite),
        interpolator(
                renderClock,
                750,
                (windowSize.x - sprite.getDeviceSize().x) / 2 + sprite.getDeviceSize().x,
                -sprite.getDeviceSize().x),
        frame(0) {

}

void trippin::SimpleOverlay::reset(int _frame) {
    frame = _frame;
    interpolator.reset();
}

void trippin::SimpleOverlay::render() {
    Point<int> pos{interpolator.interpolate(), (windowSize.y - sprite.getDeviceSize().y) / 2};
    sprite.renderDevice(pos, frame);
}