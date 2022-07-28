#include "ExitOverlay.h"

trippin::ExitOverlay::ExitOverlay(
        const Point<int> &windowSize,
        int margin,
        SpriteManager &spriteManager,
        const RenderClock &renderClock) :
        exitSprite(spriteManager.get("exit")),
        top{windowSize.y - exitSprite.getDeviceSize().y - margin},
        interpolator(renderClock, 750, margin + exitSprite.getDeviceSize().x, -exitSprite.getDeviceSize().x) {}

void trippin::ExitOverlay::reset() {
    interpolator.reset();
}

void trippin::ExitOverlay::render() {
    exitSprite.renderDevice({interpolator.interpolate(), top}, 0);
}

bool trippin::ExitOverlay::exitClicked(const trippin::Point<int> &coords) const {
    auto dims = exitSprite.getDeviceSize();
    Rect<int> r{interpolator.interpolate(), top, dims.x, dims.y};
    return r.contains(coords);
}