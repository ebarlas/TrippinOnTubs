#include "EndMenu.h"

trippin::EndMenu::EndMenu(const Point<int> &windowSize, SpriteManager &spriteManager)
        : menuLayout(windowSize, 750) {
    menuLayout.setSprite(0, &spriteManager.get("save_score"));
    menuLayout.setSprite(1, &spriteManager.get("exit"));
    menuLayout.init();
}

void trippin::EndMenu::reset() {
    menuLayout.reset();
}

void trippin::EndMenu::render() {
    menuLayout.render();
}

bool trippin::EndMenu::exitClicked(const trippin::Point<int> &coords) const {
    return menuLayout.contains(1, coords);
}

bool trippin::EndMenu::saveClicked(const trippin::Point<int> &coords) const {
    return menuLayout.contains(0, coords);
}