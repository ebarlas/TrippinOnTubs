#include "SpriteObject.h"

trippin::SpriteObject::SpriteObject(const Configuration &config, const Map::Object &object, const Sprite &sprite)
        : sprite(sprite) {
    platform = object.platform;
    id = object.id;
    lane = object.lane;
    gravity = object.gravity;
    fallGravity = object.fallGravity;
    auto hb = sprite.getHitBox();
    mass = object.mass > 0 ? object.mass : (object.massFactor > 0 ? hb.area() * object.massFactor : hb.area());
    auto objPosX = object.rightOf != 0 ? object.rightOf + sprite.getSize().x * object.rightMultiple : object.position.x;
    position = {objPosX + hb.corner().x, object.position.y + hb.corner().y};
    size = {hb.w, hb.h};
    velocity = object.velocity;
    terminalVelocity = object.terminalVelocity;
    friction = object.friction;
    velocity = object.velocity;
    syncPositions();
}
