#include "Ball.h"
#include "lock/Exchange.h"

void trippin::Ball::init(const Configuration &config, const Map::Object &obj, const Sprite &spr) {
    SpriteObject::init(config, obj, spr);
    inactive = true;
    channel.ref() = {roundedPosition, 0};
    reflectiveCollision.setCoefficient(obj.coefficient);
    platformCollision.set(&reflectiveCollision);
}

void trippin::Ball::beforeTick(Uint32 engineTicks) {
    if (inactive && activation->shouldActivate(roundedPosition.x)) {
        inactive = false;
    }
}

void trippin::Ball::afterTick(Uint32 engineTicks) {
    Exchange<Channel> ex{channel};
    auto &ch = ex.get();

    if (!universe.intersect(roundedBox)) {
        expired = true;
        return;
    }

    ch.roundedPosition = roundedPosition;
    if (engineTicks % sprite->getFramePeriodTicks() == 0) {
        ch.frame = (ch.frame + 1) % sprite->getFrames();
    }
}

void trippin::Ball::render(const trippin::Camera &camera) {
    auto ch = channel.get();
    sprite->render(ch.roundedPosition, ch.frame, camera);
}

void trippin::Ball::setActivation(const Activation *act) {
    activation = act;
}

void trippin::Ball::setUniverse(Point<int> uni) {
    universe = {0, 0, uni.x, uni.y};
}
