#include "MenuOverlay.h"

void trippin::MenuOverlay::init(const Point<int> &windowSize, SpriteManager &spriteManager) {
    menuLayout.setSprite(0, &spriteManager.get("start"));
    menuLayout.setSprite(1, &spriteManager.get("high_score"));
    menuLayout.setSprite(2, &spriteManager.get("exit"));
    menuLayout.init(windowSize, 750);
}

void trippin::MenuOverlay::reset() {
    menuLayout.reset();
}

void trippin::MenuOverlay::render() {
    menuLayout.render();
}

bool trippin::MenuOverlay::startClicked(const Point<int> &coords) const {
    return menuLayout.contains(0, coords);
}

bool trippin::MenuOverlay::exitClicked(const trippin::Point<int> &coords) const {
    return menuLayout.contains(2, coords);
}

bool trippin::MenuOverlay::highScoreClicked(const trippin::Point<int> &coords) const {
    return menuLayout.contains(1, coords);
}