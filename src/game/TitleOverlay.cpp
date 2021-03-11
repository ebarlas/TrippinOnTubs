#include "TitleOverlay.h"

void trippin::TitleOverlay::init(const Point<int> &windowSize, SpriteManager &spriteManager) {
    auto &title = spriteManager.get("trippin");

    position.x = (windowSize.x - title.getSize().x) / 2;
    position.y = (windowSize.y - title.getSize().y) / 2;

    sprite = &title;

    interpolator.init(2'000, position.x + title.getSize().x);
    interpolator.reset();
}

void trippin::TitleOverlay::render() {
    Point<int> pos{-sprite->getSize().x + interpolator.interpolate(), position.y};
    sprite->render(pos, 0);
}
