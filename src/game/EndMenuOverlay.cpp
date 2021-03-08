#include "EndMenuOverlay.h"

void trippin::EndMenuOverlay::init(const Point<int> &windowSize, SpriteManager &spriteManager) {
    auto &save = spriteManager.get("save_score");
    auto &exit = spriteManager.get("exit");

    auto saveSize = save.getSize();
    auto exitSize = exit.getSize();

    savePos.x = (windowSize.x - saveSize.x) / 2;
    savePos.y = (windowSize.y - (saveSize.y + exitSize.y)) / 2;

    exitPos.x = (windowSize.x - exitSize.x) / 2;
    exitPos.y = (windowSize.y - (saveSize.y + exitSize.y)) / 2 + saveSize.y;

    saveSprite = &save;
    exitSprite = &exit;
}

void trippin::EndMenuOverlay::render() {
    saveSprite->render(savePos, 0);
    exitSprite->render(exitPos, 0);
}

bool trippin::EndMenuOverlay::exitClicked(const trippin::Point<int> &coords) const {
    auto size = exitSprite->getSize();
    Rect<int> rect{exitPos.x, exitPos.y, size.x, size.y};
    return rect.contains(coords);
}