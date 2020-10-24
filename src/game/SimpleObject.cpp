#include "SimpleObject.h"
#include "Lock.h"

void trippin::SimpleObject::init(const trippin::Configuration &config, const trippin::Map::Object &obj,
                                 const trippin::Sprite &spr) {
    SpriteObject::init(config, obj, spr);
    channel.roundedPosition = roundedPosition;
}

void trippin::SimpleObject::render(SDL_Renderer *renderer, const trippin::Camera &camera) {
    auto hb = sprite->getHitBox();
    auto size = sprite->getSize();
    auto viewport = camera.getViewport();
    auto ch = getChannel();
    trippin::Rect<int> box{ch.roundedPosition.x - hb.x, ch.roundedPosition.y - hb.y, size.x, size.y};
    if (box.intersect(viewport)) {
        Point<int> target = {box.x - viewport.x, box.y - viewport.y};
        sprite->render(target, 0);
    }
}

void trippin::SimpleObject::afterTick(Uint32 engineTicks) {
    Lock lock(mutex);
    channel.roundedPosition = roundedPosition;
}

trippin::SimpleObject::Channel trippin::SimpleObject::getChannel() {
    Lock lock(mutex);
    return channel;
}
