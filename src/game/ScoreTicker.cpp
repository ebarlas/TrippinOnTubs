#include "ScoreTicker.h"
#include "ui/DigitLayout.h"

trippin::ScoreTicker::ScoreTicker(
        int margin,
        const Sprite &digits,
        int score,
        Rect<int> windowSize,
        SceneBuilder &sceneBuilder,
        int zIndex) :
        margin(margin),
        digits(digits),
        score(score),
        windowSize(windowSize),
        sceneBuilder(sceneBuilder),
        zIndex(zIndex) {
}

void trippin::ScoreTicker::add(int n) {
    score += n;
}

int trippin::ScoreTicker::getScore() const {
    return score;
}

void trippin::ScoreTicker::afterTick(Uint32) {
    auto textWidth = DigitLayout::measureWidth(digits, score);
    Point<int> pos{windowSize.w / 2 + textWidth / 2, margin};
    auto scoreNow = getScore();
    sceneBuilder.dispatch([this, scoreNow, pos]() {
        DigitLayout::renderDigits(digits, pos, scoreNow);
    }, zIndex);
}