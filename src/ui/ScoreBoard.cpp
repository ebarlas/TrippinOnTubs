#include "ScoreBoard.h"
#include "DigitLayout.h"

trippin::ScoreBoard::ScoreBoard(const Sprite &sprite) : sprite(sprite) {

}

void trippin::ScoreBoard::setScores(std::vector<Score> vec) {
    scores = std::move(vec);
}

void trippin::ScoreBoard::render(Point<int> position) {
    auto digitSize = sprite.getDeviceSize();

    auto narrow = scores.size() <= 9;

    int rankRight = position.x + digitSize.x * (narrow ? 1 : 2);
    int nameLeft = position.x + digitSize.x * (narrow ? 2 : 3);
    int scoreRight = nameLeft + digitSize.x * 9; // space for 3 chars, 1 sp, 5 digits

    int y = position.y;
    for (int i = 0; i < scores.size(); i++) {
        auto &score = scores[i];
        DigitLayout::renderDigits(sprite, {rankRight, y}, (i + 1));
        renderName({nameLeft, y}, score.name);
        DigitLayout::renderDigits(sprite, {scoreRight, y}, score.score);
        y += digitSize.y;
    }
}

void trippin::ScoreBoard::renderName(Point<int> position, const std::string &name) const {
    for (int i = 0; i < 3; i++) {
        Point<int> pos{position.x + i * sprite.getDeviceSize().x, position.y};
        sprite.renderDevice(pos, 10 + name[i] - 'A'); // 'A' is frame 10
    }
}

int trippin::ScoreBoard::getHeight() const {
    return scores.size() * sprite.getDeviceSize().y;
}

int trippin::ScoreBoard::getWidth() const {
    auto narrow = scores.size() <= 9;
    return (narrow ? 11 : 12) * sprite.getDeviceSize().x; // 1 or 2 digits, 1 sp, 3 chars, 1 sp, 5 digits
}
