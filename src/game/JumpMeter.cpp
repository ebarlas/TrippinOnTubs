#include "JumpMeter.h"
#include "engine/Convert.h"

void trippin::JumpMeter::init() {
    channel.set(sprite->getFrames() - 1);
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
    sprite->render(position, channel.get());
}

void trippin::JumpMeter::afterTick(Uint32 engineTicks) {
    auto jumpCharge = goggin->getJumpCharge();
    auto numJumpBars = sprite->getFrames() - 1;
    channel.set(toInt(jumpCharge * numJumpBars));
}
