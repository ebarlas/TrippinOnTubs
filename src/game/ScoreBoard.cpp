#include "ScoreBoard.h"

void trippin::ScoreBoard::setScores(std::vector<Score> sc) {
    scores = std::move(sc);
}

void trippin::ScoreBoard::setTop(int y) {
    top = y;
}

void trippin::ScoreBoard::init(const trippin::Point<int> &ws, trippin::SpriteManager &spriteManager) {
    sprite = &spriteManager.get("alpha");
    windowSize = ws;
}

void trippin::ScoreBoard::render() {
    int digitWidth = sprite->getSize().x;
    int digitHeight = sprite->getSize().y;
    int leftMargin = (windowSize.x - digitWidth * 14) / 2;
    int y = top;
    for (auto &score : scores) {
        renderNumber(leftMargin + digitWidth, y, score.rank);
        renderName(leftMargin + digitWidth * 3, y, score.name);
        renderNumber(windowSize.x - leftMargin - digitWidth, y, score.score);
        y += digitHeight;
    }
}

void trippin::ScoreBoard::renderNumber(int x, int y, int value) {
    do {
        auto digit = value % 10;
        value /= 10;
        Point<int> p{x, y};
        sprite->render(p, digit);
        x -= sprite->getSize().x;
    } while (value > 0);
}

void trippin::ScoreBoard::renderName(int x, int y, const std::string &name) {
    for (int i = 0; i < 5; i++) {
        Point<int> pos{x + i * sprite->getSize().x, y};
        sprite->render(pos, 10 + name[i] - 'A');
    }
}

int trippin::ScoreBoard::getHeight() const {
    return scores.size() * sprite->getSize().y;
}

int trippin::ScoreBoard::getTop() const {
    return top;
}
