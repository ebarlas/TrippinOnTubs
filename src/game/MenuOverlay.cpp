#include "MenuOverlay.h"

void trippin::MenuOverlay::init(const Point<int> &windowSize, SpriteManager &spriteManager) {
    auto &start = spriteManager.get("start");
    auto &highScore = spriteManager.get("high_score");

    startPos.x = (windowSize.x - start.getSize().x) / 2;
    startPos.y = (windowSize.y - (start.getSize().y + highScore.getSize().y)) / 2;

    highScorePos.x = (windowSize.x - highScore.getSize().x) / 2;
    highScorePos.y = (windowSize.y - (start.getSize().y + highScore.getSize().y)) / 2 + start.getSize().y;

    startSprite = &start;
    highScoreSprite = &highScore;
}

void trippin::MenuOverlay::render() {
    startSprite->render(startPos, 0);
    highScoreSprite->render(highScorePos, 0);
}

bool trippin::MenuOverlay::startClicked(const Point<int> &coords) const {
    Rect<int> startRect{startPos.x, startPos.y, startSprite->getSize().x, startSprite->getSize().y};
    return startRect.contains(coords);
}
