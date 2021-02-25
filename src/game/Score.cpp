#include "Score.h"
#include "engine/Convert.h"

void trippin::Score::init() {
    score = 0;
    channel.set(score);
}

void trippin::Score::setMargin(int mar) {
    margin = mar;
}

void trippin::Score::setSprite(const Sprite &spr) {
    digits = &spr;
}

void trippin::Score::add(int n) {
    score += n;
    channel.set(score);
}

void trippin::Score::render(const trippin::Camera &camera) {
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

void trippin::Score::afterTick(Uint32 engineTicks) {
    score += pointsPerTick;
    channel.set(score);
}

void trippin::Score::setPointsPerTick(double ppt) {
    pointsPerTick = ppt;
}
