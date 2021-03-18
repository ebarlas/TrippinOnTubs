#include "ScoreTicker.h"
#include "engine/Convert.h"

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
    int sc = toInt(channel.get());
    int x = camera.getViewport().w - margin - digits->getSize().x;
    do {
        auto digit = sc % 10;
        sc /= 10;
        Point<int> p{x, margin};
        digits->render(p, digit);
        x -= digits->getSize().x;
    } while (sc > 0);
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
