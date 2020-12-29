#include "SpiritClock.h"
#include "lock/Lock.h"

void trippin::SpiritClock::init(const Configuration &config, const Map::Object &obj, const Sprite &spr) {
    auto mul = spr.getScale().getMultiplier();
    engineTicksPerClockBar = config.engineTicksPerSpiritClockTick();
    sprite = &spr;
    position = {
            static_cast<int>(std::round(obj.position.x * mul)),
            static_cast<int>(std::round(obj.position.y * mul))};
    channel.frame = sprite->getFrames() - 1;
}

void trippin::SpiritClock::setSpirit(const trippin::Spirit *sp) {
    spirit = sp;
}

void trippin::SpiritClock::setGoggin(const trippin::Goggin *go) {
    goggin = go;
}

void trippin::SpiritClock::render(const trippin::Camera &camera) {
    sprite->render(position, getFrame());
}

void trippin::SpiritClock::afterTick(Uint32 engineTicks) {
    Lock lock(mutex);
    auto distanceAway = goggin->position.x - spirit->getPosition();
    auto ticksAway = distanceAway / spirit->getVelocity();
    auto numClockBars = sprite->getFrames() - 1;
    auto barsAway = static_cast<int>(std::round(ticksAway / engineTicksPerClockBar));
    auto barsFilled = std::min(numClockBars, std::max(0, barsAway));
    setFrame(barsFilled);
}

int trippin::SpiritClock::getFrame() {
    Lock lock(mutex);
    return channel.frame;
}

void trippin::SpiritClock::setFrame(int fr) {
    Lock lock(mutex);
    channel.frame = fr;
}
