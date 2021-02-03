#include "PacingObject.h"
#include "lock/Exchange.h"

void trippin::PacingObject::init(const Configuration &config, const Map::Object &obj, const Sprite &spr) {
    SpriteObject::init(config, obj, spr);
    inactive = true;
    runningAcceleration = obj.runningAcceleration;
    channel.ref() = {roundedPosition, 0};
}

void trippin::PacingObject::beforeTick(Uint32 engineTicks) {
    if (inactive && activation->shouldActivate(roundedPosition.x)) {
        inactive = false;
    }
}

void trippin::PacingObject::afterTick(Uint32 engineTicks) {
    Exchange<Channel> ex{channel};
    auto &ch = ex.get();

    if (!sprite->intersectsWith(roundedPosition, universe)) {
        expired = true;
        return;
    }

    ch.roundedPosition = roundedPosition;
    if (platformCollisions.testBottom() || objectCollisions.testBottom()) {
        acceleration.x = runningAcceleration;
        if (engineTicks % sprite->getFramePeriodTicks() == 0) {
            ch.frame = (ch.frame + 1) % sprite->getFrames();
        }
    } else {
        acceleration.x = 0;
    }
}

void trippin::PacingObject::render(const trippin::Camera &camera) {
    auto ch = channel.get();
    sprite->render(ch.roundedPosition, ch.frame, camera);
}

void trippin::PacingObject::setActivation(const Activation *act) {
    activation = act;
}

void trippin::PacingObject::setUniverse(Point<int> uni) {
    universe = {0, 0, uni.x, uni.y};
}
