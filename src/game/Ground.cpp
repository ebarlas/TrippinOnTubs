#include "Ground.h"

void trippin::Ground::init(const Configuration &config, const Map::Object &obj, const Sprite &spr) {
    SpriteObject::init(config, obj, spr);
    frame = 0;
    channel.set({roundedPosition, frame, false});
    melting = false;
    inactive = true;
}

void trippin::Ground::beforeTick(Uint32 engineTicks) {
    if (inactive && activation->shouldActivate(roundedBox)) {
        inactive = false;
    }
}

void trippin::Ground::afterTick(Uint32 engineTicks) {
    ticks++;
    if (!melting && position.x <= spirit->getPosition()) {
        melting = true;
        meltingTick = ticks;
    }
    if (melting) {
        auto diff = ticks - meltingTick;
        if (diff % sprite->getFramePeriodTicks() == 0 && frame < sprite->getFrames()) {
            frame++;
        }
        if (frame == sprite->getFrames() - 1) {
            expired = true;
        }
    }
    channel.set({roundedPosition, frame, expired});
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