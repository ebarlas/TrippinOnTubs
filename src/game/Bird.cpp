#include "Bird.h"

void trippin::Bird::init(const Configuration &config, const Map::Object &obj, const Sprite &spr) {
    SpriteObject::init(config, obj, spr);
    inactive = true;
    acceleration.x = obj.runningAcceleration;
    frame = 0;
    channel.set({roundedPosition, frame});
}

void trippin::Bird::beforeTick(Uint32 engineTicks) {
    if (inactive && activation->shouldActivate(roundedBox)) {
        inactive = false;
    }
}

void trippin::Bird::afterTick(Uint32 engineTicks) {
    // early exit if not activated yet
    if (inactive) {
        return;
    }

    if (activation->shouldDeactivate(roundedBox)) {
        expired = true;
        return;
    }

    if (engineTicks % sprite->getFramePeriodTicks() == 0) {
        frame = (frame + 1) % sprite->getFrames();
    }

    channel.set({roundedPosition, frame});
}

void trippin::Bird::render(const trippin::Camera &camera) {
    auto ch = channel.get();
    sprite->render(ch.roundedPosition, ch.frame, camera);
}

void trippin::Bird::setActivation(const Activation *act) {
    activation = act;
}
