#include "ScoreBoardOverlay.h"

void trippin::ScoreBoardOverlay::setScores(std::vector<Score> sc) {
    scores = std::move(sc);
}

void trippin::ScoreBoardOverlay::setTop(int y) {
    top = y;
}

void trippin::ScoreBoardOverlay::init(const trippin::Point<int> &ws, trippin::SpriteManager &spriteManager) {
    sprite = &spriteManager.get("alpha");
    windowSize = ws;
}

void trippin::ScoreBoardOverlay::render() {
    int digitWidth = sprite->getSize().x;
    int digitHeight = sprite->getSize().y;

    auto narrow = scores.size() <= 9;

    int leftMargin = (windowSize.x - digitWidth * (narrow ? 13 : 14)) / 2;
    int rankLeft = leftMargin + (narrow ? 0 : digitWidth);
    int nameLeft = leftMargin + digitWidth * (narrow ? 2 : 3);
    int scoreLeft = windowSize.x - leftMargin - digitWidth;

    int y = top;
    for (int i = 0; i < scores.size(); i++) {
        auto &score = scores[i];
        renderNumber(rankLeft, y, (i + 1));
        renderName(nameLeft, y, score.name);
        renderNumber(scoreLeft, y, score.score);
        y += digitHeight;
    }
}

void trippin::ScoreBoardOverlay::renderNumber(int x, int y, int value) {
    do {
        auto digit = value % 10;
        value /= 10;
        Point<int> p{x, y};
        sprite->render(p, digit);
        x -= sprite->getSize().x;
    } while (value > 0);
}

void trippin::ScoreBoardOverlay::renderName(int x, int y, const std::string &name) {
    for (int i = 0; i < 5; i++) {
        Point<int> pos{x + i * sprite->getSize().x, y};
        sprite->render(pos, 10 + name[i] - 'A');
    }
}

int trippin::ScoreBoardOverlay::getHeight() const {
    return scores.size() * sprite->getSize().y;
}

int trippin::ScoreBoardOverlay::getTop() const {
    return top;
}
