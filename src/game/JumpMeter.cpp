#include "JumpMeter.h"
#include "lock/Lock.h"

void trippin::JumpMeter::init() {
    channel.frame = sprite->getFrames() - 1;
}

void trippin::JumpMeter::setPosition(Point<int> pos) {
    position = pos;
}

void trippin::JumpMeter::setGoggin(const trippin::Goggin &go) {
    goggin = &go;
}

void trippin::JumpMeter::setSprite(const trippin::Sprite &spr) {
    sprite = &spr;
}

void trippin::JumpMeter::render(const trippin::Camera &camera) {
    sprite->render(position, getFrame());
}

void trippin::JumpMeter::afterTick(Uint32 engineTicks) {
    Lock lock(mutex);
    auto jumpCharge = goggin->getJumpCharge();
    auto numJumpBars = sprite->getFrames() - 1;
    auto barsFilled = std::round(jumpCharge * numJumpBars);
    setFrame(barsFilled);
}

int trippin::JumpMeter::getFrame() {
    Lock lock(mutex);
    return channel.frame;
}

void trippin::JumpMeter::setFrame(int fr) {
    Lock lock(mutex);
    channel.frame = fr;
}
