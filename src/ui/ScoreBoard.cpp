#include "ScoreBoard.h"
#include "DigitLayout.h"

trippin::ScoreBoard::ScoreBoard(const Sprite &sprite) : sprite(sprite) {

}

void trippin::ScoreBoard::setScores(std::vector<Score> vec) {
    scores = std::move(vec);
}

void trippin::ScoreBoard::render(Point<int> position) {
    auto digitSize = sprite.getSize();

    auto narrow = scores.size() <= 9;

    int rankLeft = position.x + (narrow ? 0 : digitSize.x);
    int nameLeft = position.x + digitSize.x * (narrow ? 2 : 3);
    int scoreLeft = nameLeft + digitSize.x * 10; // space for 5 chars, 1 sp, 4 digits

    int y = position.y;
    for (int i = 0; i < scores.size(); i++) {
        auto &score = scores[i];
        DigitLayout::renderDigits(sprite, {rankLeft, y}, (i + 1));
        renderName({nameLeft, y}, score.name);
        DigitLayout::renderDigits(sprite, {scoreLeft, y}, score.score);
        y += digitSize.y;
    }
}

void trippin::ScoreBoard::renderName(Point<int> position, const std::string &name) const {
    for (int i = 0; i < 5; i++) {
        Point<int> pos{position.x + i * sprite.getSize().x, position.y};
        sprite.render(pos, 10 + name[i] - 'A'); // 'A' is frame 10
    }
}

int trippin::ScoreBoard::getHeight() const {
    return scores.size() * sprite.getSize().y;
}

int trippin::ScoreBoard::getWidth() const {
    auto narrow = scores.size() <= 9;
    return (narrow ? 13 : 14) * sprite.getSize().x; // 1 or 2 digits, 1 sp, 5 chars, 1 sp, 5 digits
}
