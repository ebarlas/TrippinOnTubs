#include "Zombie.h"
#include "lock/Exchange.h"

void trippin::Zombie::init(const Configuration &config, const Map::Object &obj, const Sprite &spr) {
    SpriteObject::init(config, obj, spr);

    auto mul = spr.getScale().getMultiplier();
    auto gameTicksPerSecondSq = config.ticksPerSecond() * config.ticksPerSecond();

    framePeriod = sprite->getDuration() / config.tickPeriod;
    runningAcceleration = (obj.runningAcceleration / gameTicksPerSecondSq) * mul;
    channel.ref() = {roundedPosition, 0};
}

void trippin::Zombie::afterTick(Uint32 engineTicks) {
    Exchange ex{channel};
    auto &ch = ex.get();
    ch.roundedPosition = roundedPosition;
    if (platformCollisions.testBottom()) {
        acceleration.x = runningAcceleration;
        if (engineTicks % framePeriod == 0) {
            ch.frame = (ch.frame + 1) % 10;
        }
    } else {
        acceleration.x = 0;
    }
}

void trippin::Zombie::render(const trippin::Camera &camera) {
    auto ch = channel.get();
    sprite->render(ch.roundedPosition, ch.frame, camera);
}
