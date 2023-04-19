#include "SpiritClock.h"
#include "engine/Convert.h"

trippin::SpiritClock::SpiritClock(
        const Configuration &config,
        const Sprite &sprite,
        const Spirit &spirit,
        const Goggin &goggin,
        Point<int> position,
        int padding,
        SceneBuilder &sceneBuilder) :
        spirit(spirit),
        sprite(sprite),
        goggin(goggin),
        position(position),
        engineTicksPerClockBar(config.engineTicksPerSpiritClockTick()),
        padding(padding),
        sceneBuilder(sceneBuilder) {
}

void trippin::SpiritClock::afterTick(int) {
    auto frameNow = getNumClockBars();
    sceneBuilder.dispatch([this, frameNow]() {
        sprite.renderDevice(position, frameNow);
    });
}

int trippin::SpiritClock::getNumClockBars() const {
    auto distanceAway = goggin.position.x - spirit.getPosition() + padding;
    auto ticksAway = distanceAway / spirit.getVelocity();
    auto numClockBars = sprite.getFrames() - 1;
    auto barsAway = toInt(ticksAway / engineTicksPerClockBar);
    return std::min(numClockBars, std::max(0, barsAway));
}
