#include "SpriteObject.h"
#include "Lock.h"

trippin::SpriteObject::SpriteObject() {
    mutex = SDL_CreateMutex();
    if (!mutex) {
        SDL_Log("Couldn't create mutex");
        std::terminate();
    }
}

trippin::SpriteObject::~SpriteObject() {
    SDL_DestroyMutex(mutex);
}

void trippin::SpriteObject::init(
        const Configuration &config,
        const trippin::Map::Object &obj,
        const trippin::Sprite &spr) {
    auto mul = scaleMultiplier(spr.getScale());
    auto gameTicksPerSecond = 1000.0 / config.tickPeriod;
    auto gameTicksPerSecondSq = gameTicksPerSecond * gameTicksPerSecond;

    platform = obj.platform;
    sprite = &spr;
    id = obj.id;
    gravity = (obj.gravity / gameTicksPerSecondSq) * mul;
    fallGravity = (obj.fallGravity / gameTicksPerSecondSq) * mul;
    auto hb = spr.getHitBox();
    position = {obj.position.x * mul + hb.corner().x, obj.position.y * mul + hb.corner().y};
    size = {hb.w, hb.h};
    velocity = (obj.velocity / gameTicksPerSecond) * mul;
    terminalVelocity = (obj.terminalVelocity / gameTicksPerSecond) * mul;
    friction = (obj.friction / gameTicksPerSecondSq) * mul;
    updateRounded();
}