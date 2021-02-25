#include "PacingObject.h"

void trippin::PacingObject::init(const Configuration &config, const Map::Object &obj, const Sprite &spr) {
    SpriteObject::init(config, obj, spr);
    inactive = true;
    runningAcceleration = obj.runningAcceleration;
    frame = 0;
    channel.set({roundedPosition, frame});
}

void trippin::PacingObject::beforeTick(Uint32 engineTicks) {
    if (inactive && activation->shouldActivate(roundedBox)) {
        inactive = false;
    }
}

void trippin::PacingObject::afterTick(Uint32 engineTicks) {
    // early exit if not activated yet
    if (inactive) {
        return;
    }

    if (activation->shouldDeactivate(roundedBox)) {
        expired = true;
        return;
    }

    if (platformCollisions.testBottom() || objectCollisions.testBottom()) {
        acceleration.x = runningAcceleration;
        if (engineTicks % sprite->getFramePeriodTicks() == 0) {
            frame = (frame + 1) % sprite->getFrames();
        }
    } else {
        acceleration.x = 0;
    }

    channel.set({roundedPosition, frame});
}

void trippin::PacingObject::render(const trippin::Camera &camera) {
    auto ch = channel.get();
    sprite->render(ch.roundedPosition, ch.frame, camera);
}

void trippin::PacingObject::setActivation(const Activation *act) {
    activation = act;
}
