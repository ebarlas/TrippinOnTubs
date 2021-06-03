#include "ScoreTicker.h"
#include "engine/Convert.h"
#include "ui/DigitLayout.h"

void trippin::ScoreTicker::init() {
    setScore(0);
}

void trippin::ScoreTicker::setMargin(int mar) {
    margin = mar;
}

void trippin::ScoreTicker::setSprite(const Sprite &spr) {
    digits = &spr;
}

void trippin::ScoreTicker::add(int n) {
    score += n;
    channel.set(score);
}

void trippin::ScoreTicker::render(const trippin::Camera &camera) {
    int textWidth = DigitLayout::measureWidth(*digits, getScore());
    Point<int> p{camera.getViewport().w / 2 + textWidth / 2, margin};
    DigitLayout::renderDigits(*digits, p, getScore());
}

void trippin::ScoreTicker::setGoggin(const Goggin *g) {
    goggin = g;
}

int trippin::ScoreTicker::getScore() const {
    return toInt(channel.get());
}

void trippin::ScoreTicker::setScore(int sc) {
    score = sc;
    channel.set(score);
}
