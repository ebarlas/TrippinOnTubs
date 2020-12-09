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
    auto mul = spr.getScale().getMultiplier();
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
    syncPositions();

    channel.roundedPosition = roundedPosition;
}

void trippin::SpriteObject::render(SDL_Renderer *renderer, const trippin::Camera &camera) {
    if (isVisible()) {
        auto hb = sprite->getHitBox();
        auto size = sprite->getSize();
        auto viewport = camera.getViewport();
        auto pos = getPosition();
        trippin::Rect<int> box{pos.x - hb.x, pos.y - hb.y, size.x, size.y};
        if (box.intersect(viewport)) {
            Point<int> target = {box.x - viewport.x, box.y - viewport.y};
            sprite->render(target, getFrame());
        }
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
