#include "TitleMenu.h"

trippin::TitleMenu::TitleMenu(const Point<int> &windowSize, SpriteManager &spriteManager) {
    menuLayout.setSprite(0, &spriteManager.get("start"));
    menuLayout.setSprite(1, &spriteManager.get("high_score"));
    menuLayout.setSprite(2, &spriteManager.get("exit"));
    menuLayout.init(windowSize, 750);
}

void trippin::TitleMenu::reset() {
    menuLayout.reset();
}

void trippin::TitleMenu::render() {
    menuLayout.render();
}

bool trippin::TitleMenu::startClicked(const Point<int> &coords) const {
    return menuLayout.contains(0, coords);
}

bool trippin::TitleMenu::exitClicked(const trippin::Point<int> &coords) const {
    return menuLayout.contains(2, coords);
}

bool trippin::TitleMenu::highScoreClicked(const trippin::Point<int> &coords) const {
    return menuLayout.contains(1, coords);
}