#include "WingedTub.h"
#include "Exchange.h"

void trippin::WingedTub::init(const Configuration &config, const Map::Object &obj, const Sprite &spr) {
    auto mul = spr.getScale().getMultiplier();
    position = {
            static_cast<int>(std::round(obj.position.x * mul)),
            static_cast<int>(std::round(obj.position.y * mul))};
    sprite = &spr;
    hitBox = spr.getHitBox() + position;
    framePeriod = spr.getDuration() / config.tickPeriod;
    expired = false;
    channel.set({0, true});
    hitGoggin = false;
}

void trippin::WingedTub::afterTick(Uint32 engineTicks) {
    Exchange exchange(channel);
    auto &ch = exchange.get();

    // Case #1: Goggin contact
    if (!hitGoggin && hitBox.intersect(goggin->roundedBox)) {
        hitGoggin = true;
        hitTicks = 0;
        ch.frame = 10;
        score->add(100);
        return;
    }

    // Case #2: Advance dust cloud
    if (hitGoggin) {
        hitTicks++;
        if (hitTicks % framePeriod == 0) {
            ch.frame++;
        }
        if (ch.frame == sprite->getFrames()) {
            expired = true;
            ch.visible = false;
        }
        return;
    }

    // Case #3: Advance flapping wings cycle
    if (engineTicks % framePeriod == 0) {
        ch.frame = (ch.frame + 1) % 10;
    }
}

void trippin::WingedTub::setGoggin(const Goggin *g) {
    goggin = g;
}

void trippin::WingedTub::setScore(Score *sc) {
    score = sc;
}

void trippin::WingedTub::render(const trippin::Camera &camera) {
    auto ch = channel.get();
    if (ch.visible) {
        sprite->render(position, ch.frame, camera);
    }
}

bool trippin::WingedTub::isExpired() {
    return expired;
}
