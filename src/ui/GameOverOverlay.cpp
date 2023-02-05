#include "GameOverOverlay.h"

trippin::GameOverOverlay::GameOverOverlay(
        const Point<int> &windowSize,
        const Sprite &sprite,
        const RenderClock &renderClock) :
        windowSize(windowSize),
        sprite(sprite),
        interpolator(
                renderClock,
                750,
                (windowSize.x - sprite.getDeviceSize().x) / 2 + sprite.getDeviceSize().x,
                -sprite.getDeviceSize().x) {

}

void trippin::GameOverOverlay::reset() {
    interpolator.reset();
}

void trippin::GameOverOverlay::render() {
    Point<int> pos{interpolator.interpolate(), (windowSize.y - sprite.getDeviceSize().y) / 2};
    sprite.renderDevice(pos, 0);
}