#include "SpriteObject.h"

trippin::SpriteObject::SpriteObject(
        const Map::Object &object,
        const Sprite &sprite,
        const Units &units) :
        sprite(sprite) {
    platform = object.platform;
    id = object.id;
    lane = object.lane;
    gravity = object.gravity;
    fallGravity = object.fallGravity;
    auto hb = units.spriteToEngine(sprite.getHitBox());
    auto massArea = object.massFactor > 0
                    ? static_cast<int_fast64_t>(Fraction<int_fast64_t>{object.massFactor, 100} * hb.area())
                    : hb.area();
    mass = object.mass > 0 ? object.mass : massArea;
//    SDL_Log("id=%d, massFactor=%d, hb.area()=%lld, massArea=%lld, mass=%lld", id, object.massFactor, hb.area(), massArea, mass);
    position = {object.position.x + hb.corner().x, object.position.y + hb.corner().y};
    size = {hb.w, hb.h};
    velocity = object.velocity;
    terminalVelocity = object.terminalVelocity;
    friction = object.friction;
    velocity = object.velocity;
    syncPositions();
}
