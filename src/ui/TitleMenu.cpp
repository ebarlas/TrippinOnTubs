#include "TitleMenu.h"

trippin::TitleMenu::TitleMenu(
        const Point<int> &windowSize,
        SpriteManager &spriteManager,
        const RenderClock &renderClock)
        : menuLayout(windowSize, 750, renderClock) {
    menuLayout.setSprite(0, &spriteManager.get("start"));
    menuLayout.setSprite(1, &spriteManager.get("train"));
    menuLayout.setSprite(2, &spriteManager.get("high_score"));
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

bool trippin::TitleMenu::highScoreClicked(const trippin::Point<int> &coords) const {
    return menuLayout.contains(2, coords);
}