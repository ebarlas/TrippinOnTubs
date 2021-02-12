#include "Bird.h"
#include "lock/Exchange.h"

void trippin::Bird::init(const Configuration &config, const Map::Object &obj, const Sprite &spr) {
    SpriteObject::init(config, obj, spr);
    inactive = true;
    acceleration.x = obj.runningAcceleration;
    channel.ref() = {roundedPosition, 0};
}

void trippin::Bird::beforeTick(Uint32 engineTicks) {
    if (inactive && activation->shouldActivate(roundedBox)) {
        inactive = false;
    }
}

void trippin::Bird::afterTick(Uint32 engineTicks) {
    Exchange<Channel> ex{channel};
    auto &ch = ex.get();

    // early exit if not activated yet
    if (inactive) {
        return;
    }

    if (activation->shouldDeactivate(roundedBox)) {
        expired = true;
        return;
    }

    ch.roundedPosition = roundedPosition;

    if (engineTicks % sprite->getFramePeriodTicks() == 0) {
        ch.frame = (ch.frame + 1) % sprite->getFrames();
    }
}

void trippin::Bird::render(const trippin::Camera &camera) {
    auto ch = channel.get();
    sprite->render(ch.roundedPosition, ch.frame, camera);
}

void trippin::Bird::setActivation(const Activation *act) {
    activation = act;
}
