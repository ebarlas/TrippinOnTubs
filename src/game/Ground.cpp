#include "Ground.h"
#include "Lock.h"

void trippin::Ground::init(const Configuration &config, const Map::Object &obj, const Sprite &spr) {
    SpriteObject::init(config, obj, spr);
    framePeriod = sprite->getDuration() / config.tickPeriod;
    channel.roundedPosition = roundedPosition;
    channel.frame = 0;
    melting = false;
}

void trippin::Ground::afterTick(Uint32 engineTicks) {
    Lock lock(mutex);
    ticks++;
    channel.roundedPosition = roundedPosition;
    if (!melting && position.x <= spirit->getPosition()) {
        melting = true;
        meltingTick = ticks;
    }
    if (melting) {
        auto diff = ticks - meltingTick;
        if (diff % framePeriod == 0 && channel.frame < sprite->getFrames()) {
            channel.frame++;
        }
        if (channel.frame == sprite->getFrames() - 1) {
            expired = true;
        }
    }
}

void trippin::Ground::setSpirit(const trippin::Spirit *sp) {
    spirit = sp;
}

trippin::Point<int> trippin::Ground::getPosition() {
    Lock lock(mutex);
    return channel.roundedPosition;
}

int trippin::Ground::getFrame() {
    Lock lock(mutex);
    return channel.frame;
}
