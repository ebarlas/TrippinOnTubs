#include "SpiritClock.h"
#include "engine/Convert.h"

trippin::SpiritClock::SpiritClock(
        const Configuration &config,
        const Sprite &sprite,
        const Spirit &spirit,
        const Goggin &goggin,
        Point<int> position,
        int_fast64_t padding,
        SceneBuilder &sceneBuilder,
        int zIndex) :
        sprite(sprite),
        spirit(spirit),
        goggin(goggin),
        position(position),
        padding(padding),
        sceneBuilder(sceneBuilder),
        zIndex(zIndex),
        engineTicksPerClockBar(config.engineTicksPerSpiritClockTick()) {
}

void trippin::SpiritClock::afterTick(Uint32) {
    auto distanceAway = goggin.position.x - spirit.getPosition() + padding;
    auto ticksAway = distanceAway / spirit.getVelocity();
    auto numClockBars = sprite.getFrames() - 1;
    auto barsAway = engineTicksPerClockBar.flip() * static_cast<int>(ticksAway);
    auto frameNow = std::min(numClockBars, std::max(0, static_cast<int>(barsAway)));
    sceneBuilder.dispatch([this, frameNow]() {
        sprite.render(position, frameNow);
    }, zIndex);
}