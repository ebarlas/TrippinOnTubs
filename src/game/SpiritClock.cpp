#include "SpiritClock.h"
#include "engine/Convert.h"

void trippin::SpiritClock::init(const Configuration &config, const Sprite &spr) {
    engineTicksPerClockBar = config.engineTicksPerSpiritClockTick();
    sprite = &spr;
    channel.set(sprite->getFrames() - 1);
}

void trippin::SpiritClock::setSpirit(const trippin::Spirit &sp) {
    spirit = &sp;
}

void trippin::SpiritClock::setGoggin(const trippin::Goggin &go) {
    goggin = &go;
}

void trippin::SpiritClock::setPosition(trippin::Point<int> pos) {
    position = pos;
}

void trippin::SpiritClock::setPadding(double p) {
    padding = p;
}

void trippin::SpiritClock::render(const trippin::Camera &camera) {
    sprite->render(position, channel.get());
}

void trippin::SpiritClock::afterTick(Uint32 engineTicks) {
    auto distanceAway = goggin->position.x - spirit->getPosition() + padding;
    auto ticksAway = distanceAway / spirit->getVelocity();
    auto numClockBars = sprite->getFrames() - 1;
    auto barsAway = toInt(ticksAway / engineTicksPerClockBar);
    channel.set(std::min(numClockBars, std::max(0, barsAway)));
}
