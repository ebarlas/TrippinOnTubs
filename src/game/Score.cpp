#include "Score.h"
#include "lock/AutoGuard.h"
#include "engine/Convert.h"

void trippin::Score::init() {
    channel.score = 0;
    gChannel.set({channel});
}

void trippin::Score::setMargin(int mar) {
    margin = mar;
}

void trippin::Score::setSprite(const Sprite &spr) {
    digits = &spr;
}

void trippin::Score::add(int n) {
    AutoGuard<Channel> ag(channel, gChannel);
    channel.score += n;
}

void trippin::Score::render(const trippin::Camera &camera) {
    int sc = toInt(gChannel.get().score);
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
    AutoGuard<Channel> ag(channel, gChannel);
    channel.score += pointsPerTick;
}

void trippin::Score::setPointsPerTick(double ppt) {
    pointsPerTick = ppt;
}
