#include "SpriteObject.h"
#include "lock/Lock.h"

void trippin::SpriteObject::init(
        const Configuration &config,
        const trippin::Map::Object &obj,
        const trippin::Sprite &spr) {
    auto mul = spr.getScale().getMultiplier();
    auto gameTicksPerSecondSq = config.ticksPerSecond() * config.ticksPerSecond();

    platform = obj.platform;
    sprite = &spr;
    id = obj.id;
    gravity = (obj.gravity / gameTicksPerSecondSq) * mul;
    fallGravity = (obj.fallGravity / gameTicksPerSecondSq) * mul;
    auto hb = spr.getHitBox();
    position = {obj.position.x * mul + hb.corner().x, obj.position.y * mul + hb.corner().y};
    size = {hb.w, hb.h};
    velocity = (obj.velocity / config.ticksPerSecond()) * mul;
    terminalVelocity = (obj.terminalVelocity / config.ticksPerSecond()) * mul;
    friction = (obj.friction / gameTicksPerSecondSq) * mul;
    syncPositions();

    channel.roundedPosition = roundedPosition;
}

void trippin::SpriteObject::render(const trippin::Camera &camera) {
    if (isVisible()) {
        sprite->render(getPosition(), getFrame(), camera);
    }
}

void trippin::SpriteObject::afterTick(Uint32 engineTicks) {
    Lock lock(mutex);
    channel.roundedPosition = roundedPosition;
}

int trippin::SpriteObject::getFrame() {
    return 0;
}

trippin::Point<int> trippin::SpriteObject::getPosition() {
    Lock lock(mutex);
    return channel.roundedPosition;
}

bool trippin::SpriteObject::isVisible() {
    return true;
}
