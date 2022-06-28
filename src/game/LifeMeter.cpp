#include "LifeMeter.h"

trippin::LifeMeter::LifeMeter(
        int margin,
        const Sprite &sprite,
        int extraLives,
        Rect<int> viewport,
        SceneBuilder &sceneBuilder) :
        sprite(sprite),
        margin(margin),
        extraLives(extraLives),
        viewport(viewport),
        sceneBuilder(sceneBuilder) {

}

void trippin::LifeMeter::afterTick(int) {
    for (int i = 0; i < extraLives; i++) {
        Point<int> pos{viewport.w - margin - (i + 1) * sprite.getDeviceSize().x, margin};
        sceneBuilder.dispatch([this, pos]() {
            sprite.renderDevice(pos, 0);
        });
    }
}