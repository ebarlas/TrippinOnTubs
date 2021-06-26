#include "LevelOverlay.h"

trippin::LevelOverlay::LevelOverlay(
        const Point<int> &windowSize,
        SpriteManager &spriteManager,
        const RenderClock &renderClock) :
        level(0),
        windowSize(windowSize),
        sprite(spriteManager.get("level")),
        interpolator(renderClock, 750, (windowSize.x - sprite.getSize().x) / 2 + sprite.getSize().x) {

}

void trippin::LevelOverlay::setLevel(int lvl) {
    level = lvl;
    interpolator.reset();
}

void trippin::LevelOverlay::render() {
    Point<int> pos;
    pos.x = interpolator.interpolate() - sprite.getSize().x;
    pos.y = (windowSize.y - sprite.getSize().y) / 2;
    sprite.render(pos, level);
}