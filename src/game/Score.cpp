#include "Score.h"
#include "Exchange.h"

void trippin::Score::init(const Configuration &config, const Sprite &spr) {
    auto mul = spr.getScale().getMultiplier();
    digits = &spr;
    topRight = {static_cast<int>(std::round(80 * mul)), static_cast<int>(std::round(80 * mul))};
    channel.ref().score = 0;
}

void trippin::Score::add(int n) {
    Exchange<Channel> exchange{channel};
    exchange.get().score += n;
}

void trippin::Score::render(const trippin::Camera &camera) {
    auto score = channel.get().score;
    int x = camera.getViewport().w - topRight.x - digits->getSize().x;
    do {
        auto digit = score % 10;
        score /= 10;
        Point<int> p{x, topRight.y};
        digits->render(p, digit);
        x -= digits->getSize().x;
    } while (score > 0);
}
