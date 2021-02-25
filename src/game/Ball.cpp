#include "Ball.h"

void trippin::Ball::init(const Configuration &config, const Map::Object &obj, const Sprite &spr) {
    SpriteObject::init(config, obj, spr);
    inactive = true;
    frame = 0;
    channel.set({roundedPosition, frame});
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

    if (engineTicks % sprite->getFramePeriodTicks() == 0) {
        frame = (frame + 1) % sprite->getFrames();
    }

    channel.set({roundedPosition, frame});
}

void trippin::Ball::render(const trippin::Camera &camera) {
    auto ch = channel.get();
    sprite->render(ch.roundedPosition, ch.frame, camera);
}

void trippin::Ball::setActivation(const Activation *act) {
    activation = act;
}
