#include "Ground.h"
#include "Lock.h"

void trippin::Ground::init(const Configuration &config, const Map::Object &obj, const Sprite &spr) {
    SpriteObject::init(config, obj, spr);
    framePeriod = sprite->getDuration() / config.tickPeriod;
    activationTick = 150 + static_cast<int>(roundedPosition.x / (spr.getScale().getMultiplier() * 8));
    channel.roundedPosition = roundedPosition;
    channel.frame = 0;
}

void trippin::Ground::afterTick(Uint32 engineTicks) {
    Lock lock(mutex);
    ticks++;
    channel.roundedPosition = roundedPosition;
    auto diff = ticks - activationTick;
    if (diff > 0 && diff % framePeriod == 0 && channel.frame < sprite->getFrames()) {
        channel.frame++;
    }
    if (channel.frame == sprite->getFrames() - 1) {
        expired = true;
    }
}

void trippin::Ground::render(SDL_Renderer *renderer, const trippin::Camera &camera) {
    auto hb = sprite->getHitBox();
    auto size = sprite->getSize();
    auto viewport = camera.getViewport();
    auto ch = getChannel();
    trippin::Rect<int> box{ch.roundedPosition.x - hb.x, ch.roundedPosition.y - hb.y, size.x, size.y};
    if (box.intersect(viewport)) {
        Point<int> target = {box.x - viewport.x, box.y - viewport.y};
        sprite->render(target, ch.frame);
    }
}

trippin::Ground::Channel trippin::Ground::getChannel() {
    Lock lock(mutex);
    return channel;
}
