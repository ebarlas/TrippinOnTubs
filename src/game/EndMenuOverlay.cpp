#include "EndMenuOverlay.h"

void trippin::EndMenuOverlay::init(const Point<int> &windowSize, SpriteManager &spriteManager) {
    menuLayout.setSprite(0, &spriteManager.get("save_score"));
    menuLayout.setSprite(1, &spriteManager.get("exit"));
    menuLayout.init(windowSize, 750);
}

void trippin::EndMenuOverlay::reset() {
    menuLayout.reset();
}

void trippin::EndMenuOverlay::render() {
    menuLayout.render();
}

bool trippin::EndMenuOverlay::exitClicked(const trippin::Point<int> &coords) const {
    return menuLayout.contains(1, coords);
}

bool trippin::EndMenuOverlay::saveClicked(const trippin::Point<int> &coords) const {
    return menuLayout.contains(0, coords);
}
