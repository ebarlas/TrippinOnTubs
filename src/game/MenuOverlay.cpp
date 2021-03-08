#include "MenuOverlay.h"

void trippin::MenuOverlay::init(const Point<int> &windowSize, SpriteManager &spriteManager) {
    auto &start = spriteManager.get("start");
    auto &highScore = spriteManager.get("high_score");
    auto &exit = spriteManager.get("exit");

    auto startSize = start.getSize();
    auto highScoreSize = highScore.getSize();
    auto exitSize = exit.getSize();

    startPos.x = (windowSize.x - startSize.x) / 2;
    startPos.y = (windowSize.y - (startSize.y + highScoreSize.y + exitSize.y)) / 2;

    highScorePos.x = (windowSize.x - highScoreSize.x) / 2;
    highScorePos.y = (windowSize.y - (startSize.y + highScoreSize.y + exitSize.y)) / 2 + startSize.y;

    exitPos.x = (windowSize.x - exitSize.x) / 2;
    exitPos.y = (windowSize.y - (startSize.y + highScoreSize.y + exitSize.y)) / 2 + startSize.y + highScoreSize.y;

    startSprite = &start;
    highScoreSprite = &highScore;
    exitSprite = &exit;
}

void trippin::MenuOverlay::render() {
    startSprite->render(startPos, 0);
    highScoreSprite->render(highScorePos, 0);
    exitSprite->render(exitPos, 0);
}

bool trippin::MenuOverlay::startClicked(const Point<int> &coords) const {
    auto size = startSprite->getSize();
    Rect<int> startRect{startPos.x, startPos.y, size.x, size.y};
    return startRect.contains(coords);
}

bool trippin::MenuOverlay::exitClicked(const trippin::Point<int> &coords) const {
    auto size = exitSprite->getSize();
    Rect<int> rect{exitPos.x, exitPos.y, size.x, size.y};
    return rect.contains(coords);
}
