#include "JumpMeter.h"
#include "engine/Convert.h"

trippin::JumpMeter::JumpMeter(
        const Sprite &sprite,
        const Goggin &goggin,
        Point<int> position,
        SceneBuilder &sceneBuilder,
        int zIndex) :
        sprite(sprite),
        goggin(goggin),
        position(position),
        sceneBuilder(sceneBuilder),
        zIndex(zIndex) {
}

void trippin::JumpMeter::afterTick(Uint32 engineTicks) {
    auto jumpCharge = goggin.getJumpCharge();
    auto numJumpBars = sprite.getFrames() - 1;
    auto frameNow = toInt(jumpCharge * numJumpBars);
    sceneBuilder.dispatch([this, frameNow]() {
        sprite.renderDevice(position, frameNow);
    }, zIndex);
}