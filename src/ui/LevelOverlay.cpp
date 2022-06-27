#include "LevelOverlay.h"

trippin::LevelOverlay::LevelOverlay(
        const Point<int> &windowSize,
        SpriteManager &spriteManager,
        const RenderClock &renderClock) :
        level(0),
        windowSize(windowSize),
        sprite(spriteManager.get("level")),
        interpolator(
                renderClock,
                750,
                (windowSize.x - sprite.getDeviceSize().x) / 2 + sprite.getDeviceSize().x,
                -sprite.getDeviceSize().x) {

}

void trippin::LevelOverlay::setLevel(int lvl) {
    level = lvl;
    interpolator.reset();
}

void trippin::LevelOverlay::render() {
    Point<int> pos{interpolator.interpolate(), (windowSize.y - sprite.getDeviceSize().y) / 2};
    sprite.renderDevice(pos, level);
}