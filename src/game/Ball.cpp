#include "Ball.h"

void trippin::Ball::init(const Configuration &config, const Map::Object &obj, const Sprite &spr) {
    SpriteObject::init(config, obj, spr);
    inactive = true;
    frame = 0;
    syncChannel();
    reflectiveCollision.setCoefficient(obj.coefficient);
    platformCollision.set(&reflectiveCollision);
}

void trippin::Ball::beforeTick(Uint32 engineTicks) {
    if (inactive && activation->shouldActivate(roundedBox)) {
        inactive = false;
    }
}

void trippin::Ball::afterTick(Uint32 engineTicks) {
    if (inactive) {
        return;
    }

    if (activation->shouldDeactivate(roundedBox)) {
        expired = true;
        syncChannel();
        return;
    }

    if (engineTicks % sprite->getFramePeriodTicks() == 0) {
        frame = (frame + 1) % sprite->getFrames();
    }

    syncChannel();
}

void trippin::Ball::render(const trippin::Camera &camera) {
    auto ch = channel.get();
    if (ch.visible) {
        sprite->render(ch.roundedPosition, ch.frame, camera);
    }
}

void trippin::Ball::setActivation(const Activation *act) {
    activation = act;
}

void trippin::Ball::syncChannel() {
    channel.set({roundedPosition, frame, !inactive && !expired});
}
