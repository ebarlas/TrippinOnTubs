#include "RunningClock.h"
#include "lock/Exchange.h"

void trippin::RunningClock::init(const Configuration &config, const Map::Object &obj, const Sprite &spr) {
    SpriteObject::init(config, obj, spr);
    lane = -1;
    inactive = true;
    runningAcceleration = obj.runningAcceleration;
    channel.ref() = {roundedPosition, 0};
}

void trippin::RunningClock::beforeTick(Uint32 engineTicks) {
    if (inactive && activation->shouldActivate(roundedPosition.x)) {
        inactive = false;
    }
}

void trippin::RunningClock::afterTick(Uint32 engineTicks) {
    Exchange<Channel> ex{channel};
    auto &ch = ex.get();

    // early exit if not activated yet
    if (inactive) {
        return;
    }

    if (!sprite->intersectsWith(roundedPosition, universe)) {
        expired = true;
        return;
    }

    // Case #1: Goggin contact
    if (!hitGoggin && roundedBox.intersect(goggin->roundedBox)) {
        hitGoggin = true;
        hitTicks = 0;
        ch.frame = 24;
        spirit->delay(1);
        return;
    }

    // Case #2: Advance dust cloud
    if (hitGoggin) {
        hitTicks++;
        if (hitTicks % (sprite->getFramePeriodTicks() * 2) == 0) {
            ch.frame++;
        }
        if (ch.frame == sprite->getFrames()) {
            expired = true;
        }
        return;
    }

    // Case #3: Advance flapping wings cycle
    ch.roundedPosition = roundedPosition;
    if (platformCollisions.testBottom() || objectCollisions.testBottom()) {
        acceleration.x = runningAcceleration;
        if (engineTicks % sprite->getFramePeriodTicks() == 0) {
            ch.frame = (ch.frame + 1) % 24;
        }
    } else {
        acceleration.x = 0;
    }
}

void trippin::RunningClock::setGoggin(const Goggin *g) {
    goggin = g;
}

void trippin::RunningClock::setSpirit(Spirit *sp) {
    spirit = sp;
}

void trippin::RunningClock::render(const trippin::Camera &camera) {
    auto ch = channel.get();
    if (ch.frame < sprite->getFrames()) {
        sprite->render(ch.roundedPosition, ch.frame, camera);
    }
}

void trippin::RunningClock::setActivation(const Activation *act) {
    activation = act;
}

void trippin::RunningClock::setUniverse(Point<int> uni) {
    universe = {0, 0, uni.x, uni.y};
}
