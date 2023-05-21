#include "SpeedUpOverlay.h"

trippin::SpeedUpOverlay::SpeedUpOverlay(
        const Point<int> &windowSize,
        int margin,
        SpriteManager &spriteManager,
        const RenderClock &renderClock) :
        sprite(spriteManager.get("replay_speed_up")),
        top{windowSize.y - sprite.getDeviceSize().y - margin},
        interpolator(renderClock, 750, -(margin + sprite.getDeviceSize().x), windowSize.x) {}

void trippin::SpeedUpOverlay::reset() {
    interpolator.reset();
}

void trippin::SpeedUpOverlay::render() {
    sprite.renderDevice({interpolator.interpolate(), top}, 0);
}
