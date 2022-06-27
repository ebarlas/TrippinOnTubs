#include "SpriteObject.h"

static double calculateMass(const trippin::Configuration::Object &obj, const trippin::Sprite &sprite) {
    if (obj.mass > 0) {
        return obj.mass;
    }
    if (obj.massFactor > 0) {
        return sprite.getEngineHitBox().area() * obj.massFactor;
    }
    return sprite.getEngineHitBox().area();
}

trippin::SpriteObject::SpriteObject(
        const Configuration::Object &configObject,
        const Map::Object &object,
        const Sprite &sprite) :
        sprite(sprite) {
    platform = configObject.platform;
    id = object.id;
    lane = object.lane;
    gravity = configObject.gravity;
    fallGravity = configObject.fallGravity;
    mass = calculateMass(configObject, sprite);
    auto hb = sprite.getEngineHitBox();
    position = {static_cast<double>(object.position.x) + hb.corner().x,
                static_cast<double>(object.position.y) + hb.corner().y};
    size = {hb.w, hb.h};
    velocity = configObject.velocity;
    terminalVelocity = configObject.terminalVelocity;
    friction = configObject.friction;
    syncPositions();
}
