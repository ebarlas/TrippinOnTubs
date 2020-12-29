#include "JumpMeter.h"
#include "lock/Lock.h"

void trippin::JumpMeter::init(const Configuration &config, const Map::Object &obj, const Sprite &spr) {
    auto mul = spr.getScale().getMultiplier();
    sprite = &spr;
    position = {
            static_cast<int>(std::round(obj.position.x * mul)),
            static_cast<int>(std::round(obj.position.y * mul))};
    channel.frame = sprite->getFrames() - 1;
}

void trippin::JumpMeter::setGoggin(const trippin::Goggin *go) {
    goggin = go;
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
