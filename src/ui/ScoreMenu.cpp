#include "ScoreMenu.h"

trippin::ScoreMenu::ScoreMenu(const trippin::Point<int> &windowSize, trippin::SpriteManager &spriteManager)
        : menuLayout(windowSize, 750) {
    menuLayout.setSprite(0, &spriteManager.get("all_time"));
    menuLayout.setSprite(1, &spriteManager.get("today"));
    menuLayout.setSprite(2, &spriteManager.get("exit"));
    menuLayout.init();
}

void trippin::ScoreMenu::reset() {
    menuLayout.reset();
}

void trippin::ScoreMenu::render() {
    menuLayout.render();
}

bool trippin::ScoreMenu::allTimeClicked(const Point<int> &coords) const {
    return menuLayout.contains(0, coords);
}

bool trippin::ScoreMenu::todayClicked(const trippin::Point<int> &coords) const {
    return menuLayout.contains(1, coords);
}

bool trippin::ScoreMenu::exitClicked(const trippin::Point<int> &coords) const {
    return menuLayout.contains(2, coords);
}
