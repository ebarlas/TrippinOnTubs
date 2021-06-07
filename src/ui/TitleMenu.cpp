#include "TitleMenu.h"

trippin::TitleMenu::TitleMenu(const Point<int> &windowSize, SpriteManager &spriteManager)
        : menuLayout(windowSize, 750) {
    menuLayout.setSprite(0, &spriteManager.get("start"));
    menuLayout.setSprite(1, &spriteManager.get("train"));
    menuLayout.setSprite(2, &spriteManager.get("high_score"));
    menuLayout.setSprite(3, &spriteManager.get("exit"));
    menuLayout.init();
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

bool trippin::TitleMenu::trainClicked(const Point<int> &coords) const {
    return menuLayout.contains(1, coords);
}

bool trippin::TitleMenu::exitClicked(const trippin::Point<int> &coords) const {
    return menuLayout.contains(3, coords);
}

bool trippin::TitleMenu::highScoreClicked(const trippin::Point<int> &coords) const {
    return menuLayout.contains(2, coords);
}