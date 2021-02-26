#include "SpriteObject.h"

void trippin::SpriteObject::init(
        const Configuration &config,
        const trippin::Map::Object &obj,
        const trippin::Sprite &spr) {
    platform = obj.platform;
    sprite = &spr;
    id = obj.id;
    lane = obj.lane;
    gravity = obj.gravity;
    fallGravity = obj.fallGravity;
    auto hb = spr.getHitBox();
    mass = obj.mass > 0 ? obj.mass : (obj.massFactor > 0 ? hb.area() * obj.massFactor : hb.area());
    position = {obj.position.x + hb.corner().x, obj.position.y + hb.corner().y};
    size = {hb.w, hb.h};
    velocity = obj.velocity;
    terminalVelocity = obj.terminalVelocity;
    friction = obj.friction;
    velocity = obj.velocity;
    syncPositions();
}
