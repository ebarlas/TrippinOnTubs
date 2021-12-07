#include "SpiritClock.h"
#include "engine/Convert.h"

trippin::SpiritClock::SpiritClock(
        const Configuration &config,
        const Sprite &sprite,
        const Spirit &spirit,
        const Goggin &goggin,
        Point<int> position,
        double padding,
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

void trippin::SpiritClock::afterTick(Uint32 engineTicks) {
    auto distanceAway = goggin.position.x - spirit.getPosition() + padding;
    auto ticksAway = distanceAway / spirit.getVelocity();
    auto numClockBars = sprite.getFrames() - 1;
    auto barsAway = toInt(ticksAway / engineTicksPerClockBar);
    auto frameNow = std::min(numClockBars, std::max(0, barsAway));
    sceneBuilder.dispatch([this, frameNow]() {
        sprite.render(position, frameNow);
    }, zIndex);
}