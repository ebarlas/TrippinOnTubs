#include "WingedTub.h"
#include "Lock.h"

void trippin::WingedTub::init(const Configuration &config, const Map::Object &obj, const Sprite &spr) {
    SpriteObject::init(config, obj, spr);
    framePeriod = sprite->getDuration() / config.tickPeriod;
    channel.frame = 0;
    hitGoggin = false;
}

void trippin::WingedTub::afterTick(Uint32 engineTicks) {
    Lock lock(mutex);

    // Case #1: Goggin contact
    if (!hitGoggin && roundedBox.intersect(goggin->roundedBox)) {
        hitGoggin = true;
        hitTicks = 0;
        channel.frame = 10;
        return;
    }

    // Case #2: Advance dust cloud
    if (hitGoggin) {
        hitTicks++;
        if (hitTicks % framePeriod == 0) {
            channel.frame++;
        }
        if (channel.frame == sprite->getFrames()) {
            expired = true;
        }
        return;
    }

    // Case #3: Advance flapping wings cycle
    if (engineTicks % framePeriod == 0) {
        channel.frame = (channel.frame + 1) % 10;
    }
}

trippin::Point<int> trippin::WingedTub::getPosition() {
    return roundedPosition;
}

int trippin::WingedTub::getFrame() {
    Lock lock(mutex);
    return channel.frame;
}

void trippin::WingedTub::setGoggin(const Goggin *g) {
    goggin = g;
}

bool trippin::WingedTub::isVisible() {
    Lock lock(mutex);
    return channel.frame < sprite->getFrames();
}
