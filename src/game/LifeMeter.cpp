#include "LifeMeter.h"

trippin::LifeMeter::LifeMeter(
        int margin,
        const Sprite &sprite,
        int extraLives,
        Rect<int> viewport,
        SceneBuilder &sceneBuilder,
        int zIndex) :
        margin(margin),
        sprite(sprite),
        extraLives(extraLives),
        viewport(viewport),
        sceneBuilder(sceneBuilder),
        zIndex(zIndex) {

}

void trippin::LifeMeter::afterTick(Uint32 engineTicks) {
    for (int i = 0; i < extraLives; i++) {
        Point<int> pos{viewport.w - margin - (i + 1) * sprite.getDeviceSize().x, margin};
        sceneBuilder.dispatch([this, pos]() {
            sprite.renderDevice(pos, 0);
        }, zIndex);
    }
}