#include "TitleOverlay.h"

void trippin::TitleOverlay::init(const Point<int> &windowSize, SpriteManager &spriteManager) {
    auto &title = spriteManager.get("trippin");

    position.x = (windowSize.x - title.getSize().x) / 2;
    position.y = (windowSize.y - title.getSize().y) / 2;

    sprite = &title;
}

void trippin::TitleOverlay::render() {
    sprite->render(position, 0);
}
