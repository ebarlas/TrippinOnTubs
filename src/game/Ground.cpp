#include "Ground.h"
#include "lock/Exchange.h"

void trippin::Ground::init(const Configuration &config, const Map::Object &obj, const Sprite &spr) {
    SpriteObject::init(config, obj, spr);
    channel.get() = {roundedPosition, 0, false};
    melting = false;
    inactive = true;
}

void trippin::Ground::beforeTick(Uint32 engineTicks) {
    if (inactive && activation->shouldActivate(roundedPosition.x)) {
        inactive = false;
    }
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
        if (diff % sprite->getFramePeriodTicks() == 0 && ch.frame < sprite->getFrames()) {
            ch.frame++;
        }
        if (ch.frame == sprite->getFrames() - 1) {
            ch.expired = expired = true;
        }
    }
}

void trippin::Ground::render(const trippin::Camera &camera) {
    auto ch = channel.get();
    if (!ch.expired) {
        sprite->render(ch.roundedPosition, ch.frame, camera);
    }
}

void trippin::Ground::setSpirit(const trippin::Spirit *sp) {
    spirit = sp;
}

void trippin::Ground::setActivation(const Activation *act) {
    activation = act;
}