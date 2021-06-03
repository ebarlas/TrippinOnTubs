#include "LifeMeter.h"

trippin::LifeMeter::LifeMeter(int margin, const trippin::Sprite &sprite)
        : extraLives(0), margin(margin), sprite(sprite) {

}

void trippin::LifeMeter::setExtraLives(int lives) {
    extraLives = lives;
}

void trippin::LifeMeter::render(const Camera &camera) {
    for (int i = 0; i < extraLives; i++) {
        auto vp = camera.getViewport();
        Point<int> pos{vp.w - margin - (i + 1) * sprite.getSize().x, margin};
        sprite.render(pos, 0);
    }
}
