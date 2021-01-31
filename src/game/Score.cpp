#include "Score.h"
#include "lock/Exchange.h"

void trippin::Score::init() {
    channel.ref().score = 0;
}

void trippin::Score::setMargin(int mar) {
    margin = mar;
}

void trippin::Score::setSprite(const Sprite &spr) {
    digits = &spr;
}

void trippin::Score::add(int n) {
    Exchange<Channel> exchange{channel};
    exchange.get().score += n;
}

void trippin::Score::render(const trippin::Camera &camera) {
    auto score = channel.get().score;
    int x = camera.getViewport().w - margin - digits->getSize().x;
    do {
        auto digit = score % 10;
        score /= 10;
        Point<int> p{x, margin};
        digits->render(p, digit);
        x -= digits->getSize().x;
    } while (score > 0);
}
