#include "ScoreTicker.h"
#include "engine/Convert.h"
#include "ui/DigitLayout.h"

void trippin::ScoreTicker::init() {
    score = 0;
    channel.set(score);
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
    Point<int> p{camera.getViewport().w - margin - digits->getSize().x, margin};
    DigitLayout::renderDigits(*digits, p, getScore());
}

void trippin::ScoreTicker::afterTick(Uint32 engineTicks) {
    if (!goggin->expired) {
        score += pointsPerTick;
        channel.set(score);
    }
}

void trippin::ScoreTicker::setPointsPerTick(double ppt) {
    pointsPerTick = ppt;
}

void trippin::ScoreTicker::setGoggin(const Goggin *g) {
    goggin = g;
}

int trippin::ScoreTicker::getScore() const {
    return toInt(channel.get());
}

void trippin::ScoreTicker::setScore(int sc) {
    score = sc;
}
