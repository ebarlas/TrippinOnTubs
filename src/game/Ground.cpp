#include "Ground.h"
#include "lock/Exchange.h"

void trippin::Ground::init(const Configuration &config, const Map::Object &obj, const Sprite &spr) {
    SpriteObject::init(config, obj, spr);
    framePeriod = sprite->getDuration() / config.tickPeriod;
    channel.get() = {roundedPosition, 0};
    melting = false;
}

void trippin::Ground::afterTick(Uint32 engineTicks) {
    Exchange<Channel> exchange{channel};
    auto &ch = exchange.get();
    ticks++;
    ch.roundedPosition = roundedPosition;
    if (!melting && position.x <= spirit->getPosition()) {
        melting = true;
        meltingTick = ticks;
    }
    if (melting) {
        auto diff = ticks - meltingTick;
        if (diff % framePeriod == 0 && ch.frame < sprite->getFrames()) {
            ch.frame++;
        }
        if (ch.frame == sprite->getFrames() - 1) {
            expired = true;
        }
    }
}

void trippin::Ground::render(const trippin::Camera &camera) {
    auto ch = channel.get();
    sprite->render(ch.roundedPosition, ch.frame, camera);
}

void trippin::Ground::setSpirit(const trippin::Spirit *sp) {
    spirit = sp;
}
