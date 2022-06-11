#include "LifeMeter.h"

trippin::LifeMeter::LifeMeter(
        int margin,
        const Sprite &sprite,
        int extraLives,
        Rect<int> windowSize,
        SceneBuilder &sceneBuilder,
        int zIndex) :
        margin(margin),
        sprite(sprite),
        extraLives(extraLives),
        windowSize(windowSize),
        sceneBuilder(sceneBuilder),
        zIndex(zIndex) {

}

void trippin::LifeMeter::afterTick(Uint32) {
    for (int i = 0; i < extraLives; i++) {
        Point<int> pos{windowSize.w - margin - (i + 1) * sprite.getSize().x, margin};
        sceneBuilder.dispatch([this, pos]() {
            sprite.render(pos, 0);
        }, zIndex);
    }
}