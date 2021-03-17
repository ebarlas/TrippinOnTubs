#include "ScoreMenuOverlay.h"

void trippin::ScoreMenuOverlay::init(const Point<int> &windowSize, SpriteManager &spriteManager) {
    menuLayout.setSprite(0, &spriteManager.get("all_time"));
    menuLayout.setSprite(1, &spriteManager.get("today"));
    menuLayout.setSprite(2, &spriteManager.get("exit"));
    menuLayout.init(windowSize, 750);
}

void trippin::ScoreMenuOverlay::reset() {
    menuLayout.reset();
}

void trippin::ScoreMenuOverlay::render() {
    menuLayout.render();
}

bool trippin::ScoreMenuOverlay::allTimeClicked(const Point<int> &coords) const {
    return menuLayout.contains(0, coords);
}

bool trippin::ScoreMenuOverlay::todayClicked(const trippin::Point<int> &coords) const {
    return menuLayout.contains(1, coords);
}

bool trippin::ScoreMenuOverlay::exitClicked(const trippin::Point<int> &coords) const {
    return menuLayout.contains(2, coords);
}
