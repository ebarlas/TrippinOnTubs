#include "JumpMeter.h"
#include "lock/AutoGuard.h"
#include "engine/Convert.h"

void trippin::JumpMeter::init() {
    gChannel.set({sprite->getFrames() - 1});
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
    sprite->render(position, gChannel.get().frame);
}

void trippin::JumpMeter::afterTick(Uint32 engineTicks) {
    auto jumpCharge = goggin->getJumpCharge();
    auto numJumpBars = sprite->getFrames() - 1;
    gChannel.set({toInt(jumpCharge * numJumpBars)});
}
