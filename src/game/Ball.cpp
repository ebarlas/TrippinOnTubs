#include "Ball.h"
#include "lock/AutoGuard.h"

void trippin::Ball::init(const Configuration &config, const Map::Object &obj, const Sprite &spr) {
    SpriteObject::init(config, obj, spr);
    inactive = true;
    gChannel.set({roundedPosition, 0});
    reflectiveCollision.setCoefficient(obj.coefficient);
    platformCollision.set(&reflectiveCollision);
}

void trippin::Ball::beforeTick(Uint32 engineTicks) {
    if (inactive && activation->shouldActivate(roundedBox)) {
        inactive = false;
    }
}

void trippin::Ball::afterTick(Uint32 engineTicks) {
    // early exit if not activated yet
    if (inactive) {
        return;
    }

    if (activation->shouldDeactivate(roundedBox)) {
        expired = true;
        return;
    }

    AutoGuard<Channel> ag(channel, gChannel);
    channel.roundedPosition = roundedPosition;
    if (engineTicks % sprite->getFramePeriodTicks() == 0) {
        channel.frame = (channel.frame + 1) % sprite->getFrames();
    }
}

void trippin::Ball::render(const trippin::Camera &camera) {
    auto ch = gChannel.get();
    sprite->render(ch.roundedPosition, ch.frame, camera);
}

void trippin::Ball::setActivation(const Activation *act) {
    activation = act;
}
