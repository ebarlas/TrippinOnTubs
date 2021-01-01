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
    mass = obj.mass > 0 ? obj.mass : hb.area();
    position = {obj.position.x * mul + hb.corner().x, obj.position.y * mul + hb.corner().y};
    size = {hb.w, hb.h};
    velocity = (obj.velocity / config.ticksPerSecond()) * mul;
    terminalVelocity = (obj.terminalVelocity / config.ticksPerSecond()) * mul;
    friction = (obj.friction / gameTicksPerSecondSq) * mul;
    syncPositions();
}
